#ifndef GF_PANEL_H
#define GF_PANEL_H

#include "draw.h"
#include "synth.h"
#include "tuning.h"

enum { GF_VISIBLE_PATCHES = 5, GF_PATCH_JUMP = 10, GF_BUILDER_COLUMNS = 6 };
typedef enum { GF_TUNING_CLOSED, GF_TUNING_MENU, GF_TUNING_BUILDER } TuningOverlay;

typedef struct {
    float width, height;
    bool touch;
    Rect scene, bank, controls[GF_CONTROL_COUNT], keys, previous, next, jump_back, jump_forward;
} Layout;

typedef struct {
    const GF_Bank *bank;
    const Mesh *mesh, *prism;
    size_t selected, voices;
    const GF_Tuning *tuning, *draft;
    int octave, tuning_index, tuning_focus;
    TuningOverlay tuning_overlay;
    const unsigned *held;
    const float *controls;
    float peak, time;
    const char *status;
} PanelView;

Layout panel_layout(bool touch, bool landscape);
typedef struct { Rect select, build, lower, octave, raise; } TuningBar;
typedef struct { Rect frame, lower, root, raise, degrees, cancel, apply; } TuningBuilder;

TuningBar panel_tuning_bar(const Layout *layout);
TuningBuilder panel_tuning_builder(const Layout *layout);
Rect panel_tuning_option(const Layout *layout, int index);
Rect panel_tuning_degree(const TuningBuilder *builder, int index);
Rect panel_key(const Layout *layout, int index);
int panel_note(const Layout *layout, float x, float y);
size_t panel_first_patch(size_t selected, size_t count);
Rect panel_patch_row(const Layout *layout, size_t row);
Rect panel_track(const Layout *layout, int index);
void panel_draw_tuning(const Layout *layout, const PanelView *view);
void panel_draw(const Layout *layout, const PanelView *view);

#endif
