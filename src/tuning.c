#include "tuning.h"

#include <stdio.h>

#define DEGREE(n) (UINT32_C(1) << (n))
#define MAJOR (DEGREE(0) | DEGREE(4) | DEGREE(8) | DEGREE(10) | DEGREE(14) | DEGREE(18) | DEGREE(22))
#define OCTAVE_DEGREES (DEGREE(GF_DEGREES) - 1)
#define CHROMATIC UINT32_C(0x555555)

const char gf_play_keys[GF_PLAY_KEYS + 1] = "ZXCVBNMASDFGHJKLQWERTYUIOP1234567890";

const GF_Tuning gf_tunings[GF_CUSTOM_TUNING] = {
    {MAJOR, 0},
    {DEGREE(0) | DEGREE(4) | DEGREE(6) | DEGREE(10) | DEGREE(14) | DEGREE(16) | DEGREE(20), 4},
    {DEGREE(0) | DEGREE(2) | DEGREE(8) | DEGREE(10) | DEGREE(14) | DEGREE(16) | DEGREE(20), 2},
    {DEGREE(0) | DEGREE(3) | DEGREE(6) | DEGREE(10) | DEGREE(14) | DEGREE(16) | DEGREE(20), 2},
    {DEGREE(0) | DEGREE(4) | DEGREE(8) | DEGREE(14) | DEGREE(18), 0},
    {DEGREE(0) | DEGREE(6) | DEGREE(10) | DEGREE(14) | DEGREE(20), 9},
    {DEGREE(0) | DEGREE(6) | DEGREE(10) | DEGREE(12) | DEGREE(14) | DEGREE(20), 7},
    {CHROMATIC, 0}
};

const char *const gf_tuning_names[GF_TUNING_COUNT] = {
    "C Major (Dur)", "E Minor (Moll)", "D Hijaz (Arabic)", "D Bayati (Arabic)",
    "C Major Pentatonic", "A Minor Pentatonic", "G Blues", "C Chromatic", "Custom"
};

unsigned gf_tuning_size(uint32_t degrees)
{
    unsigned count = 0;
    for (; degrees; degrees &= degrees - 1) ++count;
    return count;
}

bool gf_tuning_valid(const GF_Tuning *t)
{
    return t->root >= 0 && t->root < 12 && (t->degrees & DEGREE(0)) &&
           !(t->degrees & ~OCTAVE_DEGREES) && gf_tuning_size(t->degrees) >= GF_MIN_DEGREES;
}

static int scale_offset(uint32_t degrees, int index)
{
    const int count = (int)gf_tuning_size(degrees);
    const int octaves = index / count;
    int remaining = index % count;
    int degree = 0;
    for (;;) {
        if ((degrees & DEGREE(0)) && remaining-- == 0) break;
        degrees >>= 1;
        ++degree;
    }
    return octaves * 1200 + degree * 50;
}

int gf_tuning_octave(const GF_Tuning *t, int requested)
{
    const int span = scale_offset(t->degrees, GF_PLAY_KEYS - 1);
    const int highest = (12700 - 100 * t->root - span) / 1200 - 1;
    if (requested < -1) return -1;
    if (requested > highest) return highest;
    return requested;
}

int gf_tuning_cents(const GF_Tuning *t, int octave, int index)
{
    return (octave + 1) * 1200 + t->root * 100 + scale_offset(t->degrees, index);
}

const char *gf_pitch_class(int pitch_class)
{
    static const char *const names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return names[pitch_class];
}

void gf_pitch_label(int cents, char *label, size_t size)
{
    const int midi = cents / 100;
    snprintf(label, size, "%s%d%s", gf_pitch_class(midi % 12), midi / 12 - 1, cents % 100 ? "+" : "");
}
