#include "synth.h"

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
        CHECK(synth.voices[0].stage == GF_SUSTAIN);
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
    CHECK(synth.voices[0].stage == GF_RELEASE);
    gf_note_on(&synth, 42, NAN);
    CHECK(synth.voices[2].stage == GF_RELEASE);
    gf_all_notes_off(&synth);
    gf_render(&synth, output, 1);
    CHECK(gf_active_voices(&synth) == 0);
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

static void lua_bank(const char *asset)
{
    char error[256] = "initial";
    GF_Bank bank;
    FILE *file = fopen(asset, "rb");
    CHECK(file);
    char source[8192];
    const size_t length = fread(source, 1, sizeof(source), file);
    CHECK(feof(file) && !ferror(file));
    CHECK(fclose(file) == 0);
    CHECK(gf_bank_parse(source, length, &bank, error, sizeof(error)));
    CHECK(bank.count == 5 && !error[0]);
    CHECK(strcmp(bank.patches[0].name, "Glass Canopy") == 0);
    CHECK(strcmp(bank.patches[0].sample_file, "canopy.wav") == 0);
    CHECK(bank.patches[4].detune == -1200);
    CHECK(bank.patches[3].wave_b == GF_NOISE);
    const GF_Bank last_good = bank;
    const struct { const char *lua, *error; } invalid[] = {
        {"return ][", "unexpected"},
        {"return 7", "return a table"},
        {"return {}", "1 to 24"},
        {"local bank = {}; for i = 1, 25 do bank[i] = {} end; return bank", "1 to 24"},
        {"return {3}", "patch 1"},
        {"return {{wave_a='no-such-wave'}}", "unrecognized waveform"},
        {"return {{name=2}}", "nonempty text"},
        {"return {{name=''}}", "nonempty text"},
        {"return {{name='a\\0b'}}", "nonempty text"},
        {"return {{name='abcdefghijklmnopqrstuvwxyz0123456789'}}", "nonempty text"},
        {"return {{color='1'}}", "must be numeric"},
        {"return {{morph=2}}", "allowed range"},
        {"return {{attack=-.01}}", "allowed range"},
        {"return {{color=0/0}}", "allowed range"},
        {"return {{sample_file='../sample.wav'}}", "must be a filename"},
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
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    synthesis();
    notes_and_controls();
    sample_and_delay();
    lua_bank(argv[1]);
    return 0;
}
