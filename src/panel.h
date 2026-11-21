#ifndef GF_PANEL_H
#define GF_PANEL_H

#include "draw.h"
#include "synth.h"

enum { GF_KEY_COUNT = 13, GF_WHITE_COUNT = 8, GF_VISIBLE_PATCHES = 5, GF_PATCH_JUMP = 10 };

typedef struct {
    float width, height;
    bool touch;
    Rect scene, bank, controls[GF_CONTROL_COUNT], keys, previous, next, jump_back, jump_forward;
} Layout;

typedef struct {
    const GF_Bank *bank;
    const Mesh *mesh, *prism;
    size_t selected, voices;
    int octave;
    const unsigned *held;
    const float *controls;
    float peak, time;
    const char *status;
} PanelView;

Layout panel_layout(bool touch, bool landscape);
Rect panel_key(const Layout *layout, int semitone);
int panel_note(const Layout *layout, float x, float y);
size_t panel_first_patch(size_t selected, size_t count);
Rect panel_patch_row(const Layout *layout, size_t row);
Rect panel_track(const Layout *layout, int index);
void panel_draw(const Layout *layout, const PanelView *view);

#endif
