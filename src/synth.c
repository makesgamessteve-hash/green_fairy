#include "synth.h"

#include <math.h>
#include <string.h>

#define TAU 6.283185307179586f

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
        .fm = .4f, .lfo = .27f, .motion = .19f
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
        .note = note, .age = ++s->age, .stage = GF_ATTACK,
        .frequency = 440 * exp2f((note - 69) / 12.0f),
        .velocity = clamp(velocity, 0, 1)
    };
}

static void release_voice(GF_Synth *s, GF_Voice *voice)
{
    if (voice->stage == GF_RELEASE) return;
    voice->stage = GF_RELEASE;
    voice->release_step = voice->level / fmaxf(1, s->patch.release * s->rate);
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

static float envelope(GF_Voice *voice, const GF_Patch *patch, float rate)
{
    switch (voice->stage) {
    case GF_ATTACK:
        voice->level += 1 / fmaxf(1, patch->attack * rate);
        if (voice->level >= 1) {
            voice->level = 1;
            voice->stage = GF_DECAY;
        }
        break;
    case GF_DECAY:
        voice->level -= (1 - patch->sustain) / fmaxf(1, patch->decay * rate);
        if (voice->level <= patch->sustain + 1e-6f) {
            voice->level = patch->sustain;
            voice->stage = GF_SUSTAIN;
        }
        break;
    case GF_SUSTAIN: break;
    case GF_RELEASE:
        voice->level -= voice->release_step;
        if (voice->level <= 1e-6f) {
            voice->level = 0;
            voice->note = -1;
        }
        break;
    }
    return voice->level;
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
    case GF_NOISE:
        s->random ^= s->random << 13;
        s->random ^= s->random >> 17;
        s->random ^= s->random << 5;
        return (float)(s->random >> 8) / 8388608.0f - 1;
    }
    return 0;
}

static float render_voice(GF_Synth *s, GF_Voice *v, float morph, float detune)
{
    const GF_Patch *p = &s->patch;
    const float step_a = fminf(v->frequency / s->rate, .49f);
    const float step_b = fminf(step_a * detune, .49f);
    const float modulation = p->fm * sinf(TAU * v->phase_b) / TAU;
    const float a = oscillator(s, p->wave_a, wrap(v->phase_a + modulation), step_a);
    const float b = oscillator(s, p->wave_b, v->phase_b, step_b);
    float output = a + (b - a) * morph;
    if (s->sample_length && v->sample_pos < s->sample_length) {
        const size_t index = (size_t)v->sample_pos;
        const size_t next = index + 1 < s->sample_length ? index + 1 : index;
        const float value = s->sample[index] +
            (s->sample[next] - s->sample[index]) * (v->sample_pos - index);
        output += value * p->sample;
        v->sample_pos += step_a * s->rate / 261.625565f;
    }
    v->phase_a = wrap(v->phase_a + step_a);
    v->phase_b = wrap(v->phase_b + step_b);
    return output * v->velocity * envelope(v, p, (float)s->rate);
}

void gf_render(GF_Synth *s, float *stereo, size_t frames)
{
    const GF_Patch *p = &s->patch;
    const float detune = exp2f(p->detune / 1200);
    const float cutoff = fminf(16000, 130 * exp2f(7 * p->color));
    const float filter_amount = 1 - expf(-TAU * cutoff / s->rate);
    const size_t delay_length = s->rate * 3u / 8u;
    for (size_t i = 0; i < frames; ++i) {
        const float motion = sinf(s->lfo_phase * TAU) * p->motion;
        s->lfo_phase = wrap(s->lfo_phase + p->lfo / s->rate);
        const float morph = clamp(p->morph + motion, 0, 1);
        float dry = 0;
        for (size_t j = 0; j < GF_VOICES; ++j) {
            GF_Voice *voice = &s->voices[j];
            if (voice->note >= 0) dry += render_voice(s, voice, morph, detune);
        }
        s->filter += filter_amount * (dry * .16f - s->filter);
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
