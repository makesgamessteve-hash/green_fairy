#include "panel.h"

#include <math.h>
#include <stdio.h>

static const char *const labels[] = {"MORPH", "COLOR", "ECHO"};
static const char *const hints[] = {"OSCILLATOR BLEND", "FILTER CUTOFF", "STEREO DELAY"};

Layout panel_layout(bool touch, bool landscape)
{
    if (touch && landscape) {
        return (Layout){
            .width = 820, .height = 454, .touch = true,
            .scene = {16, 71, 474, 210}, .bank = {505, 71, 299, 98},
            .controls = {{505, 179, 93, 102}, {608, 179, 93, 102}, {711, 179, 93, 102}},
            .keys = {19, 328, 782, 91}, .previous = {565, 114, 46, 46}, .next = {698, 114, 46, 46},
            .jump_back = {513, 114, 46, 46}, .jump_forward = {750, 114, 46, 46}
        };
    }
    if (touch) {
        return (Layout){
            .width = 420, .height = 740, .touch = true,
            .scene = {16, 80, 388, 190}, .bank = {16, 282, 388, 94},
            .controls = {{16, 386, 124, 96}, {148, 386, 124, 96}, {280, 386, 124, 96}},
            .keys = {19, 527, 382, 171}, .previous = {84, 321, 52, 46}, .next = {284, 321, 52, 46},
            .jump_back = {26, 321, 52, 46}, .jump_forward = {342, 321, 52, 46}
        };
    }
    return (Layout){
        .width = 1000, .height = 740, .scene = {26, 107, 629, 332}, .bank = {673, 107, 301, 332},
        .controls = {{26, 457, 304, 103}, {348, 457, 304, 103}, {670, 457, 304, 103}},
        .keys = {39, 612, 922, 86}, .previous = {741, 388, 36, 35}, .next = {870, 388, 36, 35},
        .jump_back = {690, 388, 45, 35}, .jump_forward = {912, 388, 45, 35}
    };
}

size_t panel_first_patch(size_t selected, size_t count)
{
    if (count <= GF_VISIBLE_PATCHES || selected < 2) return 0;
    const size_t end = count - GF_VISIBLE_PATCHES;
    return selected - 2 < end ? selected - 2 : end;
}

Rect panel_patch_row(const Layout *layout, size_t row)
{
    return (Rect){layout->bank.x + 16, layout->bank.y + 97 + row * 34,
                  layout->bank.w - 32, 31};
}

Rect panel_type_filter(const Layout *layout)
{
    const Rect r = layout->bank;
    return (Rect){r.x + (layout->touch ? 9 : 17), r.y + (layout->touch ? 6 : 10),
                  layout->touch ? 106 : 169, layout->touch ? 31 : 24};
}

Rect panel_favorite(const Layout *layout)
{
    const Rect r = layout->bank, filter = panel_type_filter(layout);
    return (Rect){r.x + r.w - (layout->touch ? 43 : 56), filter.y,
                  layout->touch ? 34 : 39, filter.h};
}

Rect panel_type_option(const Layout *layout, GF_PresetType type)
{
    const Rect anchor = panel_type_filter(layout);
    const float h = layout->height < 500 ? 23 : 26;
    return (Rect){anchor.x, anchor.y + anchor.h + 4 + type * h, 177, h};
}

Rect panel_track(const Layout *layout, int index)
{
    const Rect r = layout->controls[index];
    const float padding = layout->touch ? 12 : 18;
    return (Rect){r.x + padding, r.y + (layout->touch ? 60 : 65), r.w - 2 * padding, 8};
}

static void center_text(float x, float y, float width, float size, Color c, const char *label)
{
    text(x + (width - text_width(size, label)) / 2, y, size, c, label);
}

static void fit_text(float x, float y, float width, float size, Color c, const char *label)
{
    const float space = text_width(size, label);
    text(x, y, space > width ? size * width / space : size, c, label);
}

static void instrument_panel(Rect r)
{
    shade(r, ink(0x1b2d28), ink(0x111f1b));
    frame(r, ink(0x34483d));
    box((Rect){r.x + 1, r.y + 1, r.w - 2, 1}, ink(0x415749));
}

static void header(const Layout *l, const PanelView *v)
{
    const float x = l->touch ? 16 : 26;
    const float size = l->touch ? 31 : 43;
    const float right = l->width - x;
    const Color accent = ink(0xd7ed83);
    sprite((Rect){x, 21, l->touch ? 30 : 39, l->touch ? 30 : 39}, 0, accent);
    text(x + (l->touch ? 39 : 55), 17, size, ink(0xeaf0d8), "GREEN FAIRY");
    text(x + (l->touch ? 40 : 57), l->touch ? 52 : 63,
         l->touch ? 10 : 13, ink(0xa4b4a2), "POLYPHONIC SOFTWARE SYNTH");
    const Rect output = {right - (l->touch ? 88 : 139), l->touch ? 25 : 33, l->touch ? 88 : 139, 31};
    box(output, ink(0x182822));
    frame(output, ink(0x394c42));
    sprite((Rect){output.x + 9, output.y + 12, 7, 7}, 2, v->peak > .001f ? accent : ink(0x72977d));
    text(output.x + 23, output.y + 9, l->touch ? 11 : 13, ink(0xb9c9b2), l->touch ? "12 VOICE" : "12 VOICE / DSP");
}

static void ellipse(float x, float y, float a, float b, float tilt, Color color)
{
    const float c = cosf(tilt), s = sinf(tilt);
    float last_x = x + a * c, last_y = y + a * s;
    for (int i = 1; i <= 52; ++i) {
        const float phase = i * .12083049f;
        const float horizontal = a * cosf(phase), vertical = b * sinf(phase);
        const float px = x + horizontal * c - vertical * s;
        const float py = y + horizontal * s + vertical * c;
        line(last_x, last_y, px, py, 1, color);
        last_x = px;
        last_y = py;
    }
}

static void visualizer(const Layout *l, const PanelView *v)
{
    const Rect r = l->scene;
    const GF_Scene *scene = &v->bank->scenes[v->selected];
    const Color tint = ink(scene->tint), accent = ink(scene->accent);
    const Color highlight = blend(accent, ink(0xe8efd9), .22f);
    const Color field_top = blend(ink(0x080f17), tint, .08f);
    const Color field_bottom = blend(ink(0x101c24), tint, .17f);
    const Color grid = blend(field_bottom, accent, .17f);
    const float cx = r.x + r.w * .51f, cy = r.y + r.h * .47f;
    const float scale = r.h * .215f;
    const float orbit = fminf(r.w * .30f, r.h * .40f) * (.89f + v->controls[GF_ECHO] * .20f);
    const float bottom = r.y + r.h - 41;
    const float horizon = r.y + r.h * .74f;
    shade(r, field_top, field_bottom);
    for (int i = -8; i <= 8; ++i) {
        const float x = r.x + 10 + (i + 8) * (r.w - 20) / 16;
        line(cx + (x - cx) * .25f, horizon, x, bottom, 1, grid);
    }
    for (int i = 0; i < 7; ++i) {
        const float progress = i / 6.f;
        const float y = horizon + (bottom - horizon) * progress * progress;
        line(r.x + 10, y, r.x + r.w - 10, y, 1, grid);
    }
    for (unsigned i = 0; i < 55; ++i) {
        const unsigned hash = (i + 1) * 0x9e3779b9u ^ (scene->tint * 0x85ebca6bu);
        const float sx = r.x + 15 + (r.w - 30) * ((hash >> 8) & 1023) / 1024.f;
        const float sy = r.y + 38 + (r.h - 99) * ((hash >> 21) & 1023) / 1024.f;
        const float twinkle = .29f + .20f * sinf(v->time * 1.1f + i * 1.5f);
        const float size = i % 7 == 0 ? 9 : 2;
        sprite((Rect){sx, sy, size, size}, i % 7 == 0 ? 1 : 2, alpha(highlight, twinkle));
    }

    const float ring_tilt = -.18f - scene->tilt * .70f;
    ellipse(cx, cy, orbit * 1.25f, orbit * .43f, ring_tilt, blend(field_bottom, tint, .28f));
    ellipse(cx, cy + 3, orbit, orbit * .34f, ring_tilt, blend(field_bottom, accent, .51f));
    ellipse(cx, cy + 3, orbit * .80f, orbit * .29f, -ring_tilt + .18f,
            blend(field_bottom, accent, .32f));

    const float time = v->time;
    const float pulse = sqrtf(fminf(1, v->peak));
    const float floating = (3 + 4 * v->controls[GF_MORPH]) * sinf(time * 1.2f);
    const MeshPose pose = {
        .yaw = time * scene->spin + .35f,
        .tilt = scene->tilt + .10f * sinf(time * .81f),
        .aspect = scene->aspect
    };
    const Color body = blend(tint, highlight, .09f + .16f * v->controls[GF_COLOR] + .27f * pulse);
    mesh_draw(v->mesh, cx, cy + floating, scale * (1 + pulse * .16f), pose, body, highlight);
    for (unsigned i = 0; i < scene->satellites; ++i) {
        const float phase = time * (.28f + .16f * scene->spin) + i * 6.2831853f / scene->satellites;
        const float x = cx + orbit * (1.06f + (i % 3) * .07f) * cosf(phase);
        const float y = cy + orbit * (.34f * sinf(phase) + .66f * ring_tilt * cosf(phase));
        const MeshPose fragment = {phase * 1.75f, -scene->tilt, .84f};
        mesh_draw(v->prism, x, y, scale * (.095f + (i % 3) * .023f), fragment, accent, highlight);
    }
    for (int i = 0; i < (int)r.h / 5; ++i)
        box((Rect){r.x + 2, r.y + i*5 + 2, r.w - 4, 1}, alpha(ink(0x07130f), .29f));
    frame(r, blend(field_bottom, accent, .31f));
    const float pad = l->touch ? 12 : 17;
    sprite((Rect){r.x + pad, r.y + 16, 8, 8}, 2, highlight);
    text(r.x + pad + 15, r.y + 11, 15, ink(0xb5c6ab), "SIGNAL FIELD");
    const GF_Patch *patch = &v->bank->patches[v->selected];
    static const char *const modes[] = {"STEREO", "LOW-PASS", "BAND-PASS", "HIGH-PASS"};
    const char *mode = patch->grain.density > 0 ? "GRANULAR" : modes[patch->filter.mode];
    text(r.x + r.w - pad - text_width(13, mode), r.y + 11, 13, highlight, mode);
    box((Rect){r.x + 1, bottom, r.w - 2, 40}, field_bottom);
    box((Rect){r.x + 1, bottom, r.w - 2, 1}, grid);
    text(r.x + pad, bottom + 13, 13, ink(0xb0bd9d), "OUTPUT");
    const int bars = l->touch ? 15 : 24;
    const float step = l->touch ? 11 : 15;
    const float gain = sqrtf(fminf(1, v->peak));
    for (int i = 0; i < bars; ++i) {
        const bool lit = i + .4f < gain * bars;
        const Color c = lit ? (i > bars * .78f ? ink(0xeac078) : highlight) : grid;
        box((Rect){r.x + pad + 59 + i * step, bottom + 15, step - 3, 8}, c);
    }
    char voice[24];
    snprintf(voice, sizeof(voice), "%02d / 12", (int)v->voices);
    text(r.x + r.w - pad - 61, bottom + 12, 14, ink(0xa6bc9a), voice);
}

static void button(Rect r, const char *label)
{
    shade(r, ink(0x293b30), ink(0x1a2a23));
    frame(r, ink(0x526553));
    center_text(r.x, r.y + (r.h - 20) / 2, r.w, 20, ink(0xd8e4bd), label);
}

static Color preset_accent(const PanelView *v)
{
    return blend(ink(v->bank->scenes[v->selected].accent), ink(0xe5ead2), .32f);
}

static void type_menu(const Layout *layout, const PanelView *v)
{
    const Color accent = preset_accent(v), pale = ink(0xe5ead2);
    for (int i = 0; i < v->catalog->type_count; ++i) {
        const GF_PresetType type = (GF_PresetType)i;
        const Rect r = panel_type_option(layout, type);
        const bool focus = type == v->type_focus;
        box(r, focus ? ink(0x344a3b) : ink(0x182922));
        frame(r, focus ? accent : ink(0x34493e));
        text(r.x + 10, r.y + (r.h - 13) / 2, 13, type == v->type_filter ? accent : pale,
             gf_type_names[i]);
    }
}

static void patches(const Layout *layout, const PanelView *v)
{
    const Rect r = layout->bank;
    const GF_Catalog *catalog = v->catalog;
    const Color highlighted = preset_accent(v), pale = ink(0xe5ead2), muted = ink(0x9fb2a3);
    const char *name = v->bank->patches[v->selected].name;
    const Rect filter = panel_type_filter(layout), favorite = panel_favorite(layout);
    instrument_panel(r);
    box(filter, ink(0x293b30));
    frame(filter, v->type_menu ? highlighted : ink(0x526553));
    text(filter.x + 7, filter.y + (filter.h - 11) / 2, 11, pale, gf_type_names[v->type_filter]);
    text(filter.x + filter.w - 12, filter.y + (filter.h - 11) / 2, 11, highlighted, "v");
    box(favorite, ink(0x293b30));
    frame(favorite, ink(0x526553));
    const bool starred = catalog->starred[v->selected];
    star(favorite.x + favorite.w / 2, favorite.y + favorite.h / 2, 8, starred ? highlighted : muted, starred);

    button(layout->jump_back, "<<");
    button(layout->previous, "<");
    button(layout->next, ">");
    button(layout->jump_forward, ">>");
    char count[24];
    const size_t position = catalog->position == SIZE_MAX ? 0 : catalog->position + 1;
    snprintf(count, sizeof(count), "%d / %d", (int)position, (int)catalog->count);
    const float count_size = layout->touch ? 12 : 13;
    const float count_y = layout->previous.y + (layout->previous.h - count_size) / 2;
    const char *empty = v->type_filter == GF_TYPE_FAVORITES ? "No favorites" : "No presets";
    center_text(r.x, count_y, r.w, count_size, muted, catalog->count || !layout->touch ? count : empty);
    if (layout->touch) {
        const float left = filter.x + filter.w + 7;
        const float width = favorite.x - left - 6;
        const float size = fminf(20, 20 * width / fmaxf(1, text_width(20, name)));
        center_text(left, filter.y + (filter.h - size) / 2, width, size, pale, name);
    } else {
        fit_text(r.x + 17, r.y + 39, r.w - 34, 30, pale, name);
        box((Rect){r.x + 17, r.y + 82, r.w - 34, 1}, ink(0x3e5041));
        if (!catalog->count) {
            center_text(r.x, r.y + 140, r.w, 16, muted, empty);
            return;
        }
        const size_t first = panel_first_patch(catalog->position, catalog->count);
        for (size_t row = 0; row < GF_VISIBLE_PATCHES && first + row < catalog->count; ++row) {
            const size_t index = catalog->indices[first + row];
            const Rect p = panel_patch_row(layout, row);
            const bool active = index == v->selected;
            box(p, active ? blend(ink(0x182820), highlighted, .17f) : ink(0x14221d));
            frame(p, ink(active ? 0x667955 : 0x293a31));
            if (active) box((Rect){p.x, p.y, 3, p.h}, highlighted);
            char number[12];
            snprintf(number, sizeof(number), "%03d", (int)(first + row + 1));
            text(p.x + 11, p.y + 8, 14, active ? highlighted : muted, number);
            fit_text(p.x + 54, p.y + 7, p.w - 78, 16, active ? pale : muted,
                     v->bank->patches[index].name);
            if (catalog->starred[index]) star(p.x + p.w - 13, p.y + p.h / 2, 5, highlighted, true);
        }
    }
}

static void parameters(const Layout *l, const PanelView *v)
{
    const Color accent = blend(ink(v->bank->scenes[v->selected].accent), ink(0xe5ead2), .27f);
    for (int i = 0; i < GF_CONTROL_COUNT; ++i) {
        const Rect r = l->controls[i], t = panel_track(l, i);
        instrument_panel(r);
        text(t.x, r.y + 14, 16, ink(0xe0e8cf), labels[i]);
        if (!l->touch) text(t.x, r.y + 35, 11, ink(0x9bad9b), hints[i]);
        char value[12];
        snprintf(value, sizeof(value), "%02d%%", (int)lroundf(v->controls[i] * 100));
        if (l->touch) text(t.x, r.y + 34, 12, ink(0xaabfa3), value);
        else text(r.x + r.w - 18 - text_width(17, value), r.y + 15, 17, accent, value);
        for (int mark = 0; mark <= 10; ++mark) {
            const float x = t.x + t.w * mark / 10;
            box((Rect){x, t.y + 13, 1, mark % 5 == 0 ? 7 : 4}, ink(0x4c6352));
        }
        box(t, ink(0x0b1713));
        frame(t, ink(0x455a48));
        const float progress = t.w * v->controls[i];
        box((Rect){t.x, t.y, progress, t.h}, blend(ink(0x354a3c), accent, .62f));
        box((Rect){t.x + progress - 4, t.y - 6, 8, t.h + 12}, accent);
    }
}

void panel_draw(const Layout *l, const PanelView *v)
{
    shade((Rect){0, 0, l->width, l->height}, ink(0x14231e), ink(0x0e1816));
    header(l, v);
    visualizer(l, v);
    patches(l, v);
    parameters(l, v);
    panel_draw_tuning(l, v);
    if (v->type_menu) type_menu(l, v);
}
