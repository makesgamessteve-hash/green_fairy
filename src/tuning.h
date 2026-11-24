#ifndef GF_TUNING_H
#define GF_TUNING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum { GF_PLAY_KEYS = 36, GF_DEGREES = 24, GF_MIN_DEGREES = 4,
       GF_CUSTOM_TUNING = 8, GF_TUNING_COUNT = GF_CUSTOM_TUNING + 1 };

typedef struct {
    uint32_t degrees;
    int root;
} GF_Tuning;

extern const char gf_play_keys[GF_PLAY_KEYS + 1];
extern const GF_Tuning gf_tunings[GF_CUSTOM_TUNING];
extern const char *const gf_tuning_names[GF_TUNING_COUNT];

unsigned gf_tuning_size(uint32_t degrees);
bool gf_tuning_valid(const GF_Tuning *tuning);
int gf_tuning_octave(const GF_Tuning *tuning, int requested);
int gf_tuning_cents(const GF_Tuning *tuning, int octave, int index);
const char *gf_pitch_class(int pitch_class);
void gf_pitch_label(int cents, char *label, size_t size);

#endif
