#include "synth.h"
#include "tuning.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { \
    if (!(expr)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); exit(1); } \
} while (0)

static GF_Synth synth;
static float output[16000];

static float energy(const float *data, size_t size)
{
    float total = 0;
    for (size_t i = 0; i < size; ++i) {
        CHECK(isfinite(data[i]));
        CHECK(fabsf(data[i]) <= 1);
        total += fabsf(data[i]);
    }
    return total;
}

static void synthesis(void)
{
    gf_init(&synth, 1);
    CHECK(synth.rate == 48000);
    gf_init(&synth, GF_MAX_RATE + 1);
    CHECK(synth.rate == 48000);
    gf_init(&synth, 8000);
    CHECK(synth.rate == 8000);
    CHECK(gf_active_voices(&synth) == 0);
    gf_all_notes_off(&synth);
    gf_render(&synth, output, 10);
    CHECK(energy(output, 20) == 0);

    GF_Patch patch = gf_default_patch();
    patch.attack = .001f;
    patch.decay = .001f;
    patch.release = .004f;
    patch.echo = 0;
    patch.motion = 0;
    for (int wave = GF_SINE; wave <= GF_NOISE; ++wave) {
        patch.wave_a = patch.wave_b = (GF_Wave)wave;
        gf_set_patch(&synth, &patch);
        gf_note_on(&synth, 69, 2);
        CHECK(synth.voices[0].velocity == 1);
        CHECK(synth.voices[0].frequency == 440);
        gf_render(&synth, output, 500);
        CHECK(energy(output, 1000) > 1);
        CHECK(synth.voices[0].amp.stage == GF_SUSTAIN);
        gf_note_off(&synth, 69);
        gf_note_off(&synth, 69);
        gf_render(&synth, output, 500);
        CHECK(gf_active_voices(&synth) == 0);
    }
    patch.wave_a = patch.wave_b = (GF_Wave)99;
    gf_set_patch(&synth, &patch);
    gf_note_on(&synth, 69, 1);
    gf_render(&synth, output, 20);
    CHECK(energy(output, 40) == 0);
}

static void notes_and_controls(void)
{
    gf_init(&synth, 96000);
    gf_control(&synth, GF_MORPH, -1);
    gf_control(&synth, GF_COLOR, 2);
    gf_control(&synth, GF_ECHO, .25f);
    gf_control(&synth, GF_CONTROL_COUNT, .8f);
    CHECK(synth.patch.morph == 0 && synth.patch.color == 1 && synth.patch.echo == .25f);
    gf_control(&synth, GF_COLOR, NAN);
    CHECK(synth.patch.color == 0);
    gf_note_on(&synth, -1, 1);
    gf_note_on(&synth, GF_NOTES, 1);
    gf_note_off(&synth, -1);
    gf_note_off(&synth, GF_NOTES);
    CHECK(gf_active_voices(&synth) == 0);
    for (int note = 40; note < 40 + GF_VOICES; ++note) gf_note_on(&synth, note, .5f);
    CHECK(gf_active_voices(&synth) == GF_VOICES);
    gf_note_on(&synth, 42, .7f);
    CHECK(synth.voices[2].velocity == .7f);
    gf_note_on(&synth, 90, 1);
    CHECK(synth.voices[0].note == 90);
    gf_note_on(&synth, 90, 0);
    CHECK(synth.voices[0].amp.stage == GF_RELEASE);
    gf_note_on(&synth, 42, NAN);
    CHECK(synth.voices[2].amp.stage == GF_RELEASE);
    gf_all_notes_off(&synth);
    gf_render(&synth, output, 1);
    CHECK(gf_active_voices(&synth) == 0);
}

static void tuning(void)
{
    const char keys[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < sizeof(keys) - 1; ++i) {
        const char *match = strchr(gf_play_keys, keys[i]);
        CHECK(match && !strchr(match + 1, keys[i]));
    }

    CHECK(gf_tuning_cents(&gf_tunings[0], 2, 0) == 3600);
    CHECK(gf_tuning_cents(&gf_tunings[0], 2, 35) == 9600);
    CHECK(gf_tuning_cents(&gf_tunings[1], 2, 1) == 4200);
    CHECK(gf_tuning_cents(&gf_tunings[2], 2, 1) == 3900);
    CHECK(gf_tuning_cents(&gf_tunings[3], 2, 1) == 3950);
    CHECK(gf_tuning_cents(&gf_tunings[4], 2, 35) == 12000);

    for (int i = 0; i < GF_CUSTOM_TUNING; ++i) {
        const GF_Tuning *scale = &gf_tunings[i];
        CHECK(gf_tuning_valid(scale));
        for (int octave = -1; octave <= 9; ++octave) {
            const int start = gf_tuning_octave(scale, octave);
            int last = -1;
            for (int key = 0; key < GF_PLAY_KEYS; ++key) {
                const int cents = gf_tuning_cents(scale, start, key);
                CHECK(cents > last && cents <= 12700);
                last = cents;
            }
        }
    }

    const GF_Tuning min = {0x841001, 11};
    CHECK(gf_tuning_valid(&min));
    CHECK(gf_tuning_size(min.degrees) == GF_MIN_DEGREES);
    CHECK(gf_tuning_octave(&min, -200) == -1);
    CHECK(gf_tuning_octave(&min, 2) == -1);
    CHECK(gf_tuning_cents(&min, -1, 0) == 1100);
    CHECK(gf_tuning_cents(&min, -1, 35) <= 12700);
    CHECK(!gf_tuning_valid(&(GF_Tuning){0xff, -1}));
    CHECK(!gf_tuning_valid(&(GF_Tuning){0xff, 12}));
    CHECK(!gf_tuning_valid(&(GF_Tuning){0xfe, 0}));
    CHECK(!gf_tuning_valid(&(GF_Tuning){0x10000ff, 0}));
    CHECK(!gf_tuning_valid(&(GF_Tuning){7, 0}));
    CHECK(strcmp(gf_pitch_class(9), "A") == 0);
    char label[16];
    gf_pitch_label(3600, label, sizeof(label));
    CHECK(strcmp(label, "C2") == 0);
    gf_pitch_label(3950, label, sizeof(label));
    CHECK(strcmp(label, "D#2+") == 0);

    gf_init(&synth, 48000);
    gf_note_on_pitch(&synth, 1, NAN, 1);
    gf_note_on_pitch(&synth, 1, -.5f, 1);
    gf_note_on_pitch(&synth, 1, 127.5f, 1);
    CHECK(gf_active_voices(&synth) == 0);
    gf_note_on_pitch(&synth, 0, 69, 1);
    gf_note_on_pitch(&synth, 1, 69.5f, .9f);
    CHECK(synth.voices[0].frequency == 440);
    CHECK(fabsf(synth.voices[1].frequency / 440 - exp2f(1.f / 24)) < 1e-6f);
    gf_render(&synth, output, 1500);
    CHECK(energy(output, 3000) > 1);
    gf_note_off(&synth, 1);
    CHECK(synth.voices[1].amp.stage == GF_RELEASE && synth.voices[0].amp.stage != GF_RELEASE);
}

static void sample_and_delay(void)
{
    const float sample[] = {1, .7f, .2f, 0};
    GF_Patch patch = gf_default_patch();
    patch.attack = patch.decay = patch.release = 0;
    patch.sample = patch.echo = 1;
    patch.wave_a = patch.wave_b = (GF_Wave)99;
    gf_init(&synth, 8000);
    gf_set_patch(&synth, &patch);
    gf_set_sample(&synth, NULL, 4);
    CHECK(synth.sample_length == 0);
    gf_set_sample(&synth, sample, 4);
    gf_note_on(&synth, 48, 1);
    gf_render(&synth, output, 3040);
    CHECK(energy(output, 20) > 0);
    CHECK(fabsf(output[6000] - output[6001]) > .001f);
    CHECK(synth.delay_pos == 40);
    gf_set_patch(&synth, &patch);
    CHECK(synth.delay_l[0] == 0 && synth.delay_r[0] == 0);
    CHECK(synth.sample == NULL && synth.peak == 0);
}

static float filtered_tone(GF_Patch patch, GF_FilterMode mode, int note)
{
    gf_init(&synth, 48000);
    patch.filter.mode = mode;
    gf_set_patch(&synth, &patch);
    gf_note_on(&synth, note, 1);
    gf_render(&synth, output, 6000);
    return energy(output + 6000, 6000);
}

static void resonant_filter(void)
{
    GF_Patch patch = gf_default_patch();
    patch.wave_a = patch.wave_b = GF_SINE;
    patch.fm = patch.motion = patch.echo = patch.color = 0;
    patch.attack = patch.decay = 0;
    patch.sustain = 1;
    patch.filter.resonance = 2;
    CHECK(filtered_tone(patch, GF_FILTER_LOW, 48) > 4 * filtered_tone(patch, GF_FILTER_LOW, 84));
    CHECK(filtered_tone(patch, GF_FILTER_BAND, 48) > 4 * filtered_tone(patch, GF_FILTER_BAND, 84));
    CHECK(filtered_tone(patch, GF_FILTER_HIGH, 84) > 3 * filtered_tone(patch, GF_FILTER_HIGH, 24));

    gf_init(&synth, 8000);
    patch.filter.mode = GF_FILTER_LOW;
    patch.filter.envelope = (GF_ADSR){.005f, .010f, .25f, .005f};
    patch.filter.depth = 2;
    patch.filter.velocity = .75f;
    patch.filter.keytrack = .50f;
    patch.filter.resonance = 12;
    gf_set_patch(&synth, &patch);
    gf_note_on(&synth, 60, .2f);
    gf_note_on(&synth, 72, .9f);
    gf_render(&synth, output, 40);
    CHECK(synth.voices[0].filter_env.level > .99f);
    CHECK(synth.voices[1].filter_g > synth.voices[0].filter_g);
    gf_render(&synth, output, 150);
    CHECK(synth.voices[0].filter_env.stage == GF_SUSTAIN);
    gf_note_off(&synth, 60);
    gf_render(&synth, output, 80);
    CHECK(synth.voices[0].filter_env.stage == GF_RELEASE);
    CHECK(synth.voices[0].filter_env.level == 0);
    CHECK(synth.voices[1].filter_env.stage == GF_SUSTAIN);
    gf_control(&synth, GF_COLOR, 1);
    gf_note_on(&synth, 127, 1);
    gf_render(&synth, output, 8000);
    CHECK(energy(output, 16000) > 1);
}

static void modulation_matrix(void)
{
    GF_Patch patch = gf_default_patch();
    patch.shape = (GF_Shape){.segments = {{.005f, 1}, {.005f, -1}}, .count = 2, .loop = 1};
    patch.routes[0] = (GF_ModRoute){GF_MOD_VELOCITY, GF_MOD_PITCH, 8};
    patch.routes[1] = (GF_ModRoute){GF_MOD_COLOR, GF_MOD_BLEND, .4f};
    patch.routes[2] = (GF_ModRoute){GF_MOD_SHAPE, GF_MOD_BLEND, .2f};
    patch.route_count = 3;
    gf_init(&synth, 8000);
    gf_set_patch(&synth, &patch);
    gf_note_on(&synth, 72, .75f);
    GF_Voice *voice = &synth.voices[0];
    gf_render(&synth, output, 1);
    CHECK(fabsf(voice->pitch_ratio - sqrtf(2)) < 1e-5f);
    CHECK(voice->modulation[GF_MOD_BLEND] > patch.color * .4f);
    gf_render(&synth, output, 39);
    CHECK(voice->shape_level == 1);
    gf_render(&synth, output, 40);
    CHECK(voice->shape_level == -1);
    gf_render(&synth, output, 20);
    CHECK(fabsf(voice->shape_level) < 1e-5f);
    gf_note_off(&synth, 72);
    gf_render(&synth, output, 100);
    CHECK(voice->shape_level == -1 && voice->amp.stage == GF_RELEASE);

    patch.shape.loop = 0;
    gf_set_patch(&synth, &patch);
    gf_note_on(&synth, 60, 1);
    gf_render(&synth, output, 200);
    CHECK(voice->shape_level == -1 && voice->shape_index == 2);
}

static void granular_sample(void)
{
    const float sample[] = {.8f, .55f, .3f, -.24f, -.6f, -.27f, .36f};
    GF_Patch patch = gf_default_patch();
    patch.wave_a = patch.wave_b = (GF_Wave)99;
    patch.attack = patch.decay = patch.release = patch.echo = 0;
    patch.sustain = patch.sample = 1;
    patch.grain = (GF_Granular){.24f, 100, 1, 0};
    gf_init(&synth, 8000);
    gf_set_patch(&synth, &patch);
    gf_set_sample(&synth, sample, sizeof(sample) / sizeof(*sample));
    gf_note_on(&synth, 60, 1);
    gf_render(&synth, output, 1700);
    CHECK(synth.grain_size == 1920 && synth.grain_interval == 240);
    CHECK(energy(output + 2400, 1000) > 1);
    for (size_t i = 0; i < GF_GRAINS; ++i) CHECK(synth.voices[0].grains[i].remaining);
    gf_note_off(&synth, 60);
    gf_render(&synth, output, 2);
    CHECK(gf_active_voices(&synth) == 0);

    patch.grain = (GF_Granular){.08f, 31, .40f, .32f};
    patch.routes[0] = (GF_ModRoute){GF_MOD_LFO, GF_MOD_POSITION, .3f};
    patch.route_count = 1;
    patch.filter.mode = GF_FILTER_BAND;
    float reference[512];
    gf_init(&synth, 8000);
    gf_set_patch(&synth, &patch);
    gf_set_sample(&synth, sample, sizeof(sample) / sizeof(*sample));
    gf_note_on(&synth, 72, 1);
    gf_render(&synth, reference, 256);
    CHECK(energy(reference, 512) > .01f);
    gf_init(&synth, 8000);
    gf_set_patch(&synth, &patch);
    gf_set_sample(&synth, sample, sizeof(sample) / sizeof(*sample));
    gf_note_on(&synth, 72, 1);
    gf_render(&synth, output, 43);
    gf_render(&synth, output + 86, 213);
    CHECK(memcmp(output, reference, sizeof(reference)) == 0);
}

static void factory_presets(const GF_Bank *bank)
{
    float signatures[GF_PATCHES][16], sample[128];
    size_t granular = 0, loops = 0;
    for (size_t i = 0; i < sizeof(sample) / sizeof(*sample); ++i)
        sample[i] = .45f * sinf(6.2831853f * i / 128);
    for (size_t i = 0; i < bank->count; ++i) {
        const GF_Patch *patch = &bank->patches[i];
        CHECK(patch->filter.mode != GF_FILTER_LEGACY && patch->route_count > 0);
        if (patch->grain.density > 0) {
            CHECK(patch->sample_file[0] && patch->sample > 0);
            ++granular;
        }
        if (patch->shape.loop) ++loops;
        gf_init(&synth, 48000);
        gf_set_patch(&synth, patch);
        if (patch->sample_file[0]) gf_set_sample(&synth, sample, sizeof(sample) / sizeof(*sample));
        gf_note_on(&synth, 60, .8f);
        gf_render(&synth, output, 8000);
        CHECK(energy(output, 16000) > .1f);
        for (size_t k = 0; k < 16; ++k) signatures[i][k] = output[(k + 1) * 479 * 2];
        for (size_t previous = 0; previous < i; ++previous) {
            CHECK(strcmp(patch->name, bank->patches[previous].name) != 0);
            CHECK(bank->scenes[i].tint != bank->scenes[previous].tint);
            CHECK(memcmp(signatures[i], signatures[previous], sizeof(signatures[i])) != 0);
        }
        gf_set_patch(&synth, patch);
        if (patch->sample_file[0]) gf_set_sample(&synth, sample, sizeof(sample) / sizeof(*sample));
        gf_note_on(&synth, 36, .8f);
        gf_render(&synth, output, 8000);
        CHECK(energy(output, 16000) > .1f);
    }
    CHECK(granular == 21 && loops == 36);
}

static void lua_bank(const char *asset)
{
    char error[256] = "initial";
    GF_Bank bank;
    FILE *file = fopen(asset, "rb");
    CHECK(file);
    CHECK(fseek(file, 0, SEEK_END) == 0);
    const long length = ftell(file);
    CHECK(length > 0);
    CHECK(fseek(file, 0, SEEK_SET) == 0);
    char *source = malloc((size_t)length);
    CHECK(source);
    CHECK(fread(source, 1, (size_t)length, file) == (size_t)length);
    CHECK(fclose(file) == 0);
    CHECK(gf_bank_parse(source, (size_t)length, &bank, error, sizeof(error)));
    free(source);
    CHECK(bank.count == 100 && !error[0]);
    CHECK(strcmp(bank.patches[0].name, "Glass Canopy") == 0);
    CHECK(strcmp(bank.patches[0].sample_file, "canopy.wav") == 0);
    CHECK(bank.patches[4].detune == -1200);
    CHECK(bank.patches[3].wave_b == GF_NOISE);
    CHECK(bank.patches[0].filter.mode == GF_FILTER_LOW && bank.patches[0].shape.loop);
    CHECK(bank.patches[2].filter.mode == GF_FILTER_BAND);
    CHECK(bank.patches[3].grain.density == 31 && bank.patches[3].route_count == 2);
    CHECK(strcmp(bank.patches[99].name, "Blacklight") == 0);
    CHECK(strcmp(bank.scenes[0].mesh, GF_DEFAULT_MESH) == 0);
    CHECK(strcmp(bank.scenes[99].mesh, "helix.obj") == 0);
    CHECK(bank.scenes[99].tint == 0xbc70ee);
    factory_presets(&bank);
    const GF_Bank last_good = bank;
    const struct { const char *lua, *error; } invalid[] = {
        {"return ][", "unexpected"},
        {"return 7", "return a table"},
        {"return {}", "1 to 100"},
        {"local bank = {}; for i = 1, 101 do bank[i] = {} end; return bank", "1 to 100"},
        {"return {3}", "patch 1"},
        {"return {{wave_a='no-such-wave'}}", "unrecognized value for wave_a"},
        {"return {{name=2}}", "nonempty text"},
        {"return {{name=''}}", "nonempty text"},
        {"return {{name='a\\0b'}}", "nonempty text"},
        {"return {{name='abcdefghijklmnopqrstuvwxyz0123456789'}}", "nonempty text"},
        {"return {{color='1'}}", "must be numeric"},
        {"return {{morph=2}}", "allowed range"},
        {"return {{attack=-.01}}", "allowed range"},
        {"return {{color=0/0}}", "allowed range"},
        {"return {{sample_file='../sample.wav'}}", "must be a filename"},
        {"return {{filter=5}}", "filter must be a table"},
        {"return {{filter={mode='notch'}}}", "unrecognized value for mode"},
        {"return {{filter={resonance=20}}}", "resonance is outside"},
        {"return {{mod={0}}}", "each mod route must be a table"},
        {"return {{mod={{}}}}", "unrecognized value for source"},
        {"return {{mod={{source='lfo', destination='volume'}}}}", "unrecognized value for destination"},
        {"local r = {}; for i=1,9 do r[i] = {} end; return {{mod=r}}", "mod accepts at most"},
        {"return {{shape={}}}", "shape needs 1 to"},
        {"local r = {}; for i=1,9 do r[i] = {} end; return {{shape=r}}", "shape needs 1 to"},
        {"return {{shape={loop=2, {}}}}", "loop must be boolean"},
        {"return {{shape={4}}}", "each shape segment must be a table"},
        {"return {{shape={{time=0}}}}", "time is outside"},
        {"return {{grain={}}}", "grain requires a sample_file"},
        {"return {{scene=5}}", "scene must be a table"},
        {"return {{scene={mesh='../prism.obj'}}}", "scene mesh must be an .obj filename"},
        {"return {{scene={mesh='prism'}}}", "scene mesh must be an .obj filename"},
        {"return {{scene={mesh='prism.bmp'}}}", "scene mesh must be an .obj filename"},
        {"return {{scene={tint=1.5}}}", "tint must be an integer"},
        {"return {{scene={accent=0x1000000}}}", "accent is outside its allowed range"},
        {"return {{scene={spin=4}}}", "spin is outside its allowed range"},
        {"return {{scene={tilt=-.8}}}", "tilt is outside its allowed range"},
        {"return {{scene={aspect=.2}}}", "aspect is outside its allowed range"},
        {"return {{scene={satellites=6}}}", "satellites is outside its allowed range"},
        {"return {os.execute('foo')}", "nil value"},
        {"while true do end", "instruction budget"},
    };
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        const char *lua = invalid[i].lua;
        CHECK(!gf_bank_parse(lua, strlen(lua), &bank, error, sizeof(error)));
        CHECK(strstr(error, invalid[i].error));
        CHECK(memcmp(&last_good, &bank, sizeof(bank)) == 0);
    }
    CHECK(!gf_bank_parse(NULL, 0, &bank, NULL, 0));
    CHECK(!gf_bank_parse(NULL, 0, &bank, error, 0));
    CHECK(!gf_bank_parse(NULL, 0, &bank, error, 1) && !error[0]);
    const char valid[] = "local p = {name='Math patch', morph=math.sqrt(.25)}; return {p, {}}";
    CHECK(gf_bank_parse(valid, sizeof(valid) - 1, &bank, NULL, 0));
    CHECK(gf_bank_parse(valid, sizeof(valid) - 1, &bank, error, 0));
    CHECK(bank.count == 2 && bank.patches[0].morph == .5f);
    CHECK(bank.patches[1].wave_a == GF_TRIANGLE);
    CHECK(bank.patches[1].filter.mode == GF_FILTER_LEGACY);
    CHECK(bank.patches[1].route_count == 0 && bank.patches[1].grain.density == 0);
    CHECK(strcmp(bank.scenes[1].mesh, GF_DEFAULT_MESH) == 0);
    CHECK(bank.scenes[1].tint == gf_default_scene().tint);
    const char partial[] = "return {{scene={tint=0x123456, satellites=0}}}";
    CHECK(gf_bank_parse(partial, sizeof(partial) - 1, &bank, error, sizeof(error)));
    CHECK(bank.scenes[0].tint == 0x123456 && bank.scenes[0].satellites == 0);
    CHECK(strcmp(bank.scenes[0].mesh, GF_DEFAULT_MESH) == 0);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    synthesis();
    notes_and_controls();
    tuning();
    sample_and_delay();
    resonant_filter();
    modulation_matrix();
    granular_sample();
    lua_bank(argv[1]);
    return 0;
}
