#ifndef GF_SYNTH_H
#define GF_SYNTH_H

#include <stddef.h>
#include <stdint.h>

enum { GF_VOICES = 12, GF_PATCHES = 100, GF_NAME = 32, GF_MAX_RATE = 96000,
       GF_NOTES = 128, GF_DELAY = GF_MAX_RATE / 2,
       GF_MOD_ROUTES = 8, GF_SEGMENTS = 8, GF_GRAINS = 8 };

typedef enum { GF_SINE, GF_TRIANGLE, GF_SAW, GF_SQUARE, GF_NOISE } GF_Wave;
typedef enum { GF_MORPH, GF_COLOR, GF_ECHO, GF_CONTROL_COUNT } GF_Control;
typedef enum { GF_ATTACK, GF_DECAY, GF_SUSTAIN, GF_RELEASE } GF_Stage;

typedef enum { GF_FILTER_LEGACY, GF_FILTER_LOW, GF_FILTER_BAND, GF_FILTER_HIGH } GF_FilterMode;
typedef enum { GF_MOD_LFO, GF_MOD_AMP, GF_MOD_FILTER, GF_MOD_SHAPE, GF_MOD_VELOCITY,
               GF_MOD_KEY, GF_MOD_COLOR, GF_MOD_MORPH, GF_MOD_ECHO, GF_MOD_SOURCES } GF_ModSource;
typedef enum { GF_MOD_CUTOFF, GF_MOD_PITCH, GF_MOD_BLEND, GF_MOD_FM,
               GF_MOD_SAMPLE, GF_MOD_POSITION, GF_MOD_DESTINATIONS } GF_ModDestination;

typedef struct { float attack, decay, sustain, release; } GF_ADSR;
typedef struct { GF_Stage stage; float level, release_step; } GF_Envelope;
typedef struct {
    GF_FilterMode mode;
    GF_ADSR envelope;
    float resonance, depth, velocity, keytrack;
} GF_Filter;

typedef struct { GF_ModSource source; GF_ModDestination destination; float amount; } GF_ModRoute;
typedef struct { float time, level; } GF_Segment;
typedef struct {
    GF_Segment segments[GF_SEGMENTS];
    unsigned count;
    int loop;
} GF_Shape;
typedef struct { float size, density, position, spray; } GF_Granular;

typedef struct {
    char name[GF_NAME];
    GF_Wave wave_a, wave_b;
    float attack, decay, sustain, release;
    float morph, color, echo, detune, fm, lfo, motion, sample;
    char sample_file[GF_NAME];
    GF_Filter filter;
    GF_ModRoute routes[GF_MOD_ROUTES];
    unsigned route_count;
    GF_Shape shape;
    GF_Granular grain;
} GF_Patch;

#define GF_DEFAULT_MESH "prism.obj"

typedef struct {
    char mesh[GF_NAME];
    uint32_t tint, accent;
    float spin, tilt, aspect;
    unsigned satellites;
} GF_Scene;

typedef struct {
    GF_Patch patches[GF_PATCHES];
    GF_Scene scenes[GF_PATCHES];
    size_t count;
} GF_Bank;

typedef struct { double position; unsigned remaining; } GF_Grain;
typedef struct {
    int note;
    uint64_t age;
    GF_Envelope amp, filter_env;
    float pitch, frequency, phase_a, phase_b, velocity;
    double sample_pos;
    float shape_level, shape_step;
    unsigned shape_index, shape_frames;
    float filter_g, filter_step, filter_band, filter_low;
    float modulation[GF_MOD_DESTINATIONS], pitch_ratio;
    unsigned mod_frames, grain_wait, grain_next;
    GF_Grain grains[GF_GRAINS];
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
    unsigned grain_size, grain_interval;
} GF_Synth;

GF_Patch gf_default_patch(void);
void gf_init(GF_Synth *synth, unsigned sample_rate);
void gf_set_patch(GF_Synth *synth, const GF_Patch *patch);
void gf_set_sample(GF_Synth *synth, const float *sample, size_t length);
void gf_control(GF_Synth *synth, GF_Control control, float value);
void gf_note_on(GF_Synth *synth, int note, float velocity);
void gf_note_on_pitch(GF_Synth *synth, int note, float pitch, float velocity);
void gf_note_off(GF_Synth *synth, int note);
void gf_all_notes_off(GF_Synth *synth);
void gf_render(GF_Synth *synth, float *stereo, size_t frames);
size_t gf_active_voices(const GF_Synth *synth);

GF_Scene gf_default_scene(void);
int gf_bank_parse(const char *source, size_t length, GF_Bank *bank, char *error, size_t error_size);

#endif
