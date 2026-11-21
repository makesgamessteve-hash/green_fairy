#ifndef GF_SYNTH_H
#define GF_SYNTH_H

#include <stddef.h>
#include <stdint.h>

enum { GF_VOICES = 12, GF_PATCHES = 24, GF_NAME = 32, GF_MAX_RATE = 96000,
       GF_NOTES = 128, GF_DELAY = GF_MAX_RATE / 2 };

typedef enum { GF_SINE, GF_TRIANGLE, GF_SAW, GF_SQUARE, GF_NOISE } GF_Wave;
typedef enum { GF_MORPH, GF_COLOR, GF_ECHO, GF_CONTROL_COUNT } GF_Control;
typedef enum { GF_ATTACK, GF_DECAY, GF_SUSTAIN, GF_RELEASE } GF_Stage;

typedef struct {
    char name[GF_NAME];
    GF_Wave wave_a, wave_b;
    float attack, decay, sustain, release;
    float morph, color, echo, detune, fm, lfo, motion, sample;
    char sample_file[GF_NAME];
} GF_Patch;

typedef struct {
    GF_Patch patches[GF_PATCHES];
    size_t count;
} GF_Bank;

typedef struct {
    int note;
    uint64_t age;
    GF_Stage stage;
    float frequency, phase_a, phase_b, level, velocity, release_step, sample_pos;
} GF_Voice;

typedef struct {
    GF_Patch patch;
    GF_Voice voices[GF_VOICES];
    float delay_l[GF_DELAY], delay_r[GF_DELAY];
    const float *sample;
    size_t sample_length, delay_pos;
    unsigned rate;
    uint32_t random;
    uint64_t age;
    float filter, lfo_phase, peak;
} GF_Synth;

GF_Patch gf_default_patch(void);
void gf_init(GF_Synth *synth, unsigned sample_rate);
void gf_set_patch(GF_Synth *synth, const GF_Patch *patch);
void gf_set_sample(GF_Synth *synth, const float *sample, size_t length);
void gf_control(GF_Synth *synth, GF_Control control, float value);
void gf_note_on(GF_Synth *synth, int note, float velocity);
void gf_note_off(GF_Synth *synth, int note);
void gf_all_notes_off(GF_Synth *synth);
void gf_render(GF_Synth *synth, float *stereo, size_t frames);
size_t gf_active_voices(const GF_Synth *synth);

int gf_bank_parse(const char *source, size_t length, GF_Bank *bank, char *error, size_t error_size);

#endif
