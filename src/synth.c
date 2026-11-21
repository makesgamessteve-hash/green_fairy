#include "synth.h"

#include <math.h>
#include <string.h>

#define TAU 6.283185307179586f
#define C4_HZ 261.625565f
enum { MOD_FRAMES = 16 };

static float clamp(float value, float low, float high)
{
    if (!isfinite(value)) return low;
    return fminf(high, fmaxf(low, value));
}

GF_Patch gf_default_patch(void)
{
    return (GF_Patch){
        .name = "Glass Canopy", .wave_a = GF_TRIANGLE, .wave_b = GF_SINE,
        .attack = .08f, .decay = .5f, .sustain = .65f, .release = 1.1f,
        .morph = .35f, .color = .62f, .echo = .38f, .detune = 9,
        .fm = .4f, .lfo = .27f, .motion = .19f,
        .filter = {.envelope = {.006f, .35f, .16f, .45f}, .resonance = .707f},
        .grain = {.position = .35f}
    };
}

void gf_init(GF_Synth *s, unsigned rate)
{
    memset(s, 0, sizeof(*s));
    s->rate = rate >= 8000 && rate <= GF_MAX_RATE ? rate : 48000;
    s->random = 0x3187b949u;
    GF_Patch patch = gf_default_patch();
    gf_set_patch(s, &patch);
}

void gf_set_patch(GF_Synth *s, const GF_Patch *p)
{
    s->patch = *p;
    for (size_t i = 0; i < GF_VOICES; ++i) s->voices[i].note = -1;
    s->sample = NULL;
    s->sample_length = 0;
    s->filter = s->peak = s->lfo_phase = 0;
    s->delay_pos = 0;
    s->grain_size = (unsigned)fmaxf(2, p->grain.size * s->rate);
    s->grain_interval = p->grain.density > 0
        ? (unsigned)ceilf(fmaxf(s->rate / p->grain.density, s->grain_size / (float)GF_GRAINS))
        : 0;
    memset(s->delay_l, 0, sizeof(s->delay_l));
    memset(s->delay_r, 0, sizeof(s->delay_r));
}

void gf_set_sample(GF_Synth *s, const float *sample, size_t length)
{
    s->sample = sample;
    s->sample_length = sample ? length : 0;
}

void gf_control(GF_Synth *s, GF_Control control, float value)
{
    const float amount = clamp(value, 0, 1);
    switch (control) {
    case GF_MORPH: s->patch.morph = amount; break;
    case GF_COLOR: s->patch.color = amount; break;
    case GF_ECHO: s->patch.echo = amount; break;
    default: break;
    }
}

void gf_note_on(GF_Synth *s, int note, float velocity)
{
    if (note < 0 || note >= GF_NOTES) return;
    if (!isfinite(velocity) || velocity <= 0) {
        gf_note_off(s, note);
        return;
    }
    GF_Voice *oldest = &s->voices[0];
    GF_Voice *choice = NULL;
    for (size_t i = 0; i < GF_VOICES; ++i) {
        GF_Voice *voice = &s->voices[i];
        if (voice->note == note) {
            choice = voice;
            break;
        }
        if (voice->note < 0 && !choice) choice = voice;
        if (voice->age < oldest->age) oldest = voice;
    }
    if (!choice) choice = oldest;
    *choice = (GF_Voice){
        .note = note, .age = ++s->age, .pitch_ratio = 1,
        .frequency = 440 * exp2f((note - 69) / 12.0f),
        .velocity = clamp(velocity, 0, 1)
    };
}

static void release_envelope(GF_Envelope *env, float frames)
{
    env->stage = GF_RELEASE;
    env->release_step = env->level / fmaxf(1, frames);
}

static void release_voice(GF_Synth *s, GF_Voice *voice)
{
    if (voice->amp.stage == GF_RELEASE) return;
    release_envelope(&voice->amp, s->patch.release * s->rate);
    release_envelope(&voice->filter_env, s->patch.filter.envelope.release * s->rate);
}

void gf_note_off(GF_Synth *s, int note)
{
    if (note < 0 || note >= GF_NOTES) return;
    for (size_t i = 0; i < GF_VOICES; ++i)
        if (s->voices[i].note == note) release_voice(s, &s->voices[i]);
}

void gf_all_notes_off(GF_Synth *s)
{
    for (size_t i = 0; i < GF_VOICES; ++i)
        if (s->voices[i].note >= 0) release_voice(s, &s->voices[i]);
}

size_t gf_active_voices(const GF_Synth *s)
{
    size_t count = 0;
    for (size_t i = 0; i < GF_VOICES; ++i) count += s->voices[i].note >= 0;
    return count;
}

static float envelope(GF_Envelope *env, const GF_ADSR *spec, float rate)
{
    switch (env->stage) {
    case GF_ATTACK:
        env->level += 1 / fmaxf(1, spec->attack * rate);
        if (env->level >= 1) {
            env->level = 1;
            env->stage = GF_DECAY;
        }
        break;
    case GF_DECAY:
        env->level -= (1 - spec->sustain) / fmaxf(1, spec->decay * rate);
        if (env->level <= spec->sustain + 1e-6f) {
            env->level = spec->sustain;
            env->stage = GF_SUSTAIN;
        }
        break;
    case GF_SUSTAIN: break;
    case GF_RELEASE: env->level = fmaxf(0, env->level - env->release_step); break;
    }
    return env->level;
}

static void advance_shape(GF_Voice *v, const GF_Shape *shape, unsigned rate)
{
    if (!v->shape_frames) {
        if (v->shape_index == shape->count) {
            if (!shape->loop || v->amp.stage == GF_RELEASE) return;
            v->shape_index = 0;
        }
        const GF_Segment segment = shape->segments[v->shape_index++];
        v->shape_frames = (unsigned)fmaxf(1, segment.time * rate);
        v->shape_step = (segment.level - v->shape_level) / v->shape_frames;
    }
    v->shape_level += v->shape_step;
    if (!--v->shape_frames) v->shape_level = shape->segments[v->shape_index - 1].level;
}

static float wrap(float phase)
{
    return phase - floorf(phase);
}

static float poly_blep(float phase, float step)
{
    if (phase < step) {
        const float t = phase / step;
        return t + t - t * t - 1;
    }
    if (phase > 1 - step) {
        const float t = (phase - 1) / step;
        return t * t + t + t + 1;
    }
    return 0;
}

static float noise(GF_Synth *s)
{
    s->random ^= s->random << 13;
    s->random ^= s->random >> 17;
    s->random ^= s->random << 5;
    return (float)(s->random >> 8) / 8388608.0f - 1;
}

static float oscillator(GF_Synth *s, GF_Wave wave, float phase, float step)
{
    switch (wave) {
    case GF_SINE: return sinf(TAU * phase);
    case GF_TRIANGLE: return 1 - 4 * fabsf(phase - .5f);
    case GF_SAW: return 2 * phase - 1 - poly_blep(phase, step);
    case GF_SQUARE: {
        const float sign = phase < .5f ? 1 : -1;
        return sign + poly_blep(phase, step) - poly_blep(wrap(phase + .5f), step);
    }
    case GF_NOISE: return noise(s);
    }
    return 0;
}

static float sample_at(const GF_Synth *s, double position, size_t next)
{
    const size_t index = (size_t)position;
    return s->sample[index] + (s->sample[next] - s->sample[index]) * (float)(position - index);
}

static float render_grains(GF_Synth *s, GF_Voice *v, float pitch)
{
    if (!v->grain_wait) {
        const GF_Granular *p = &s->patch.grain;
        const float jitter = p->spray > 0 ? noise(s) * p->spray : 0;
        const float start = clamp(p->position + v->modulation[GF_MOD_POSITION] + jitter, 0, 1);
        v->grains[v->grain_next] = (GF_Grain){(double)start * (s->sample_length - 1), s->grain_size};
        v->grain_next = (v->grain_next + 1) % GF_GRAINS;
        v->grain_wait = s->grain_interval;
    }
    --v->grain_wait;
    float sound = 0;
    for (unsigned i = 0; i < GF_GRAINS; ++i) {
        GF_Grain *grain = &v->grains[i];
        if (!grain->remaining) continue;
        const float phase = (s->grain_size - grain->remaining) / (s->grain_size - 1.f);
        const float ramp = 4 * phase * (1 - phase);
        const size_t next = (size_t)grain->position + 1;
        sound += ramp * ramp * sample_at(s, grain->position, next < s->sample_length ? next : 0);
        grain->position += pitch;
        if (grain->position >= s->sample_length)
            grain->position = fmod(grain->position, (double)s->sample_length);
        --grain->remaining;
    }
    return sound / fmaxf(1, .53333f * s->grain_size / s->grain_interval);
}

static void update_modulation(GF_Synth *s, GF_Voice *v, float lfo, float cutoff)
{
    const GF_Patch *p = &s->patch;
    if (p->route_count) {
        const float sources[GF_MOD_SOURCES] = {
            lfo, v->amp.level, v->filter_env.level, v->shape_level, v->velocity,
            clamp((v->note - 60) / 60.f, -1, 1), p->color, p->morph, p->echo
        };
        memset(v->modulation, 0, sizeof(v->modulation));
        for (unsigned i = 0; i < p->route_count; ++i) {
            const GF_ModRoute route = p->routes[i];
            v->modulation[route.destination] += sources[route.source] * route.amount;
        }
        v->pitch_ratio = exp2f(clamp(v->modulation[GF_MOD_PITCH], -48, 48) / 12);
    }
    if (p->filter.mode != GF_FILTER_LEGACY) {
        const GF_Filter *filter = &p->filter;
        const float octaves = v->filter_env.level * filter->depth + v->velocity * filter->velocity +
            (v->note - 60) * filter->keytrack / 12 + v->modulation[GF_MOD_CUTOFF];
        const float frequency = clamp(cutoff * exp2f(octaves), 20, fminf(18000, s->rate * .45f));
        const float target = tanf(.5f * TAU * frequency / s->rate);
        if (!v->filter_g) v->filter_g = target;
        v->filter_step = (target - v->filter_g) / MOD_FRAMES;
    }
    v->mod_frames = MOD_FRAMES;
}

static float filter_voice(GF_Voice *v, const GF_Filter *filter, float input)
{
    v->filter_g += v->filter_step;
    const float g = v->filter_g;
    const float damping = 1 / filter->resonance;
    const float band = (v->filter_band + g * (input - v->filter_low)) / (1 + g * (g + damping));
    const float low = v->filter_low + g * band;
    v->filter_band = 2 * band - v->filter_band;
    v->filter_low = 2 * low - v->filter_low;
    if (filter->mode == GF_FILTER_LOW) return low;
    if (filter->mode == GF_FILTER_BAND) return damping * band;
    return input - damping * band - low;
}

static float render_voice(GF_Synth *s, GF_Voice *v, const GF_ADSR *amp, float lfo, float detune, float cutoff)
{
    const GF_Patch *p = &s->patch;
    const float level = envelope(&v->amp, amp, (float)s->rate);
    if (p->filter.mode != GF_FILTER_LEGACY || p->route_count)
        envelope(&v->filter_env, &p->filter.envelope, (float)s->rate);
    if (p->shape.count) advance_shape(v, &p->shape, s->rate);
    if (!v->mod_frames) update_modulation(s, v, lfo, cutoff);
    --v->mod_frames;

    const float step_a = fminf(v->frequency * v->pitch_ratio / s->rate, .49f);
    const float step_b = fminf(step_a * detune, .49f);
    const float fm = clamp(p->fm + v->modulation[GF_MOD_FM], 0, 8);
    const float modulation = fm * sinf(TAU * v->phase_b) / TAU;
    const float morph = clamp(p->morph + lfo * p->motion + v->modulation[GF_MOD_BLEND], 0, 1);
    const float a = oscillator(s, p->wave_a, wrap(v->phase_a + modulation), step_a);
    const float b = oscillator(s, p->wave_b, v->phase_b, step_b);
    float output = a + (b - a) * morph;
    if (s->sample_length) {
        const float mix = clamp(p->sample + v->modulation[GF_MOD_SAMPLE], 0, 1);
        const float pitch = step_a * s->rate / C4_HZ;
        if (s->grain_interval) {
            output += render_grains(s, v, pitch) * mix;
        } else if (v->sample_pos < s->sample_length) {
            const size_t at = (size_t)v->sample_pos;
            const size_t next = at + 1 < s->sample_length ? at + 1 : at;
            output += sample_at(s, v->sample_pos, next) * mix;
            v->sample_pos += pitch;
        }
    }
    v->phase_a = wrap(v->phase_a + step_a);
    v->phase_b = wrap(v->phase_b + step_b);
    if (p->filter.mode != GF_FILTER_LEGACY) output = filter_voice(v, &p->filter, output);
    if (v->amp.stage == GF_RELEASE && level <= 1e-6f) {
        v->note = -1;
        return 0;
    }
    return output * v->velocity * level;
}

void gf_render(GF_Synth *s, float *stereo, size_t frames)
{
    const GF_Patch *p = &s->patch;
    const float detune = exp2f(p->detune / 1200);
    const float cutoff = fminf(16000, 130 * exp2f(7 * p->color));
    const float filter_amount = 1 - expf(-TAU * cutoff / s->rate);
    const size_t delay_length = s->rate * 3u / 8u;
    const GF_ADSR amp = {p->attack, p->decay, p->sustain, p->release};
    for (size_t i = 0; i < frames; ++i) {
        const float lfo = sinf(s->lfo_phase * TAU);
        s->lfo_phase = wrap(s->lfo_phase + p->lfo / s->rate);
        float dry = 0;
        for (size_t j = 0; j < GF_VOICES; ++j) {
            GF_Voice *voice = &s->voices[j];
            if (voice->note >= 0) dry += render_voice(s, voice, &amp, lfo, detune, cutoff);
        }
        if (p->filter.mode == GF_FILTER_LEGACY) s->filter += filter_amount * (dry * .16f - s->filter);
        else s->filter = dry * .16f;
        const size_t pos = s->delay_pos;
        const float left = s->delay_l[pos], right = s->delay_r[pos];
        s->delay_l[pos] = s->filter + right * p->echo * .7f;
        s->delay_r[pos] = s->filter * .68f + left * p->echo * .7f;
        s->delay_pos = (pos + 1) % delay_length;
        stereo[i * 2] = tanhf(s->filter + left * p->echo);
        stereo[i * 2 + 1] = tanhf(s->filter + right * p->echo);
        const float peak = fmaxf(fabsf(stereo[i * 2]), fabsf(stereo[i * 2 + 1]));
        s->peak = fmaxf(peak, s->peak * .9996f);
    }
}
