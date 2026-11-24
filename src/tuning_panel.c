#include "panel.h"

#include <math.h>
#include <stdio.h>

TuningBar panel_tuning_bar(const Layout *layout)
{
    const Rect keys = layout->keys;
    const float y = keys.y - 37, h = 30, right = keys.x + keys.w;
    const float select = layout->width < 450 ? 177 : 223;
    return (TuningBar){
        .select = {keys.x, y, select, h}, .build = {keys.x + select + 6, y, 69, h},
        .lower = {right - 110, y, 31, h}, .octave = {right - 79, y, 48, h},
        .raise = {right - 31, y, 31, h}
    };
}

Rect panel_tuning_option(const Layout *layout, int index)
{
    const Rect button = panel_tuning_bar(layout).select;
    const float height = layout->touch ? 30 : 28;
    return (Rect){button.x, button.y - 4 - (GF_TUNING_COUNT - index) * height, button.w, height};
}

TuningBuilder panel_tuning_builder(const Layout *layout)
{
    const float width = fminf(520, layout->width - 30);
    const float x = (layout->width - width) / 2, y = (layout->height - 346) / 2;
    return (TuningBuilder){
        .frame = {x, y, width, 346},
        .lower = {x + 16, y + 65, 42, 34}, .root = {x + 58, y + 65, 108, 34},
        .raise = {x + 166, y + 65, 42, 34}, .degrees = {x + 16, y + 117, width - 32, 150},
        .cancel = {x + 16, y + 296, 100, 35}, .apply = {x + width - 116, y + 296, 100, 35}
    };
}

Rect panel_tuning_degree(const TuningBuilder *builder, int index)
{
    const Rect grid = builder->degrees;
    const float w = grid.w / GF_BUILDER_COLUMNS, h = grid.h / (GF_DEGREES / GF_BUILDER_COLUMNS);
    return (Rect){grid.x + index % GF_BUILDER_COLUMNS * w, grid.y + index / GF_BUILDER_COLUMNS * h,
                  w - 4, h - 4};
}

Rect panel_key(const Layout *layout, int index)
{
    const Rect keys = layout->keys;
    const int columns = layout->width < 450 ? 9 : 12;
    const float w = keys.w / columns, h = keys.h / (GF_PLAY_KEYS / columns);
    return (Rect){keys.x + index % columns * w, keys.y + index / columns * h, w - 3, h - 3};
}

int panel_note(const Layout *layout, float x, float y)
{
    for (int i = 0; i < GF_PLAY_KEYS; ++i)
        if (inside(panel_key(layout, i), x, y)) return i;
    return -1;
}

static void centered(Rect r, float size, Color color, const char *message)
{
    text(r.x + (r.w - text_width(size, message)) / 2, r.y + (r.h - size) / 2, size, color, message);
}

static void button(Rect r, Color color, const char *label, bool selected)
{
    box(r, selected ? blend(ink(0x24362c), color, .20f) : ink(0x1b2b24));
    frame(r, selected ? color : ink(0x4d6354));
    centered(r, 13, color, label);
}

static void build_view(const Layout *layout, const PanelView *v, Color accent)
{
    const TuningBuilder b = panel_tuning_builder(layout);
    const GF_Tuning *draft = v->draft;
    const Color pale = ink(0xe8efd9), muted = ink(0xb8c9b4);
    box((Rect){0, 0, layout->width, layout->height}, alpha(ink(0x070f0d), .82f));
    shade(b.frame, ink(0x26392f), ink(0x14211b));
    frame(b.frame, accent);
    text(b.frame.x + 16, b.frame.y + 13, 21, pale, "TUNING BUILDER");
    text(b.frame.x + 16, b.frame.y + 42, 13, muted, "Choose your root and offsets in cents");
    button(b.lower, pale, "<", false);
    button(b.raise, pale, ">", false);
    char value[32];
    snprintf(value, sizeof(value), "ROOT %s", gf_pitch_class(draft->root));
    centered(b.root, 15, pale, value);
    for (int i = 0; i < GF_DEGREES; ++i) {
        const bool on = (draft->degrees & (UINT32_C(1) << i)) != 0;
        const Rect r = panel_tuning_degree(&b, i);
        snprintf(value, sizeof(value), "%d", i * 50);
        button(r, on ? accent : muted, value, on);
        if (v->tuning_focus == i) frame(r, pale);
    }
    const bool playable = gf_tuning_valid(draft);
    if (playable) snprintf(value, sizeof(value), "%u degrees / octave", gf_tuning_size(draft->degrees));
    else snprintf(value, sizeof(value), "Choose at least 4 degrees");
    text(b.frame.x + 16, b.frame.y + 271, 13, playable ? muted : ink(0xffd08a), value);
    button(b.cancel, muted, "CANCEL", false);
    button(b.apply, playable ? accent : ink(0x718474), "APPLY", playable);
}

void panel_draw_tuning(const Layout *layout, const PanelView *v)
{
    const Rect k = layout->keys;
    const Color pale = ink(0xe8efd9), muted = ink(0xa3b8a7);
    const Color accent = blend(ink(v->bank->scenes[v->selected].accent), pale, .32f);
    const Rect shelf = {layout->scene.x, k.y - 46, layout->width - 2 * layout->scene.x, k.h + 54};
    shade(shelf, ink(0x1b2d28), ink(0x111f1b));
    frame(shelf, ink(0x34483d));
    const TuningBar bar = panel_tuning_bar(layout);
    char label[32];
    snprintf(label, sizeof(label), "%s  v", gf_tuning_names[v->tuning_index]);
    button(bar.select, accent, label, v->tuning_overlay == GF_TUNING_MENU);
    button(bar.build, pale, "BUILD", false);
    button(bar.lower, pale, "-", false);
    button(bar.raise, pale, "+", false);
    snprintf(label, sizeof(label), "OCT %d", v->octave);
    centered(bar.octave, 12, muted, label);

    const unsigned degree_count = gf_tuning_size(v->tuning->degrees);
    for (int i = 0; i < GF_PLAY_KEYS; ++i) {
        const Rect r = panel_key(layout, i);
        const bool tonic = i % degree_count == 0, held = v->held[i] > 0;
        const Color key_fill = held ? accent : ink(tonic ? 0x324b38 : 0x1b2c25);
        const Color key_text = held ? ink(0x14231d) : pale;
        const char binding[] = {gf_play_keys[i], '\0'};
        gf_pitch_label(gf_tuning_cents(v->tuning, v->octave, i), label, sizeof(label));
        box(r, key_fill);
        frame(r, held || tonic ? accent : ink(0x384d41));
        text(r.x + 6, r.y + 4, 12, key_text, binding);
        const float font = layout->width < 450 ? 11 : 12;
        text(r.x + r.w - text_width(font, label) - 5, r.y + r.h - font - 4, font, key_text, label);
    }
    char first[16], last[16], range[40];
    gf_pitch_label(gf_tuning_cents(v->tuning, v->octave, 0), first, sizeof(first));
    gf_pitch_label(gf_tuning_cents(v->tuning, v->octave, GF_PLAY_KEYS - 1), last, sizeof(last));
    snprintf(range, sizeof(range), "%s - %s", first, last);
    if (layout->height - k.y - k.h > 27) {
        const float y = layout->height - 21;
        text(k.x + k.w - text_width(11, range), y, 11, muted, range);
        if (v->status && *v->status) {
            const float width = k.w - text_width(11, range) - 13;
            const float font = fminf(11, 11 * width / text_width(11, v->status));
            text(k.x, y, font, ink(0xffd08a), v->status);
        } else if (layout->width >= 450) {
            text(k.x, y, 11, muted, "Z TO 0   /   SHIFT ACCENT   /   LEFT-RIGHT OCTAVE");
        }
    }
    if (v->tuning_overlay == GF_TUNING_MENU) {
        for (int i = 0; i < GF_TUNING_COUNT; ++i)
            button(panel_tuning_option(layout, i), i == v->tuning_focus ? accent : pale,
                   gf_tuning_names[i], i == v->tuning_focus);
    }
    if (v->tuning_overlay == GF_TUNING_BUILDER) build_view(layout, v, accent);
}
