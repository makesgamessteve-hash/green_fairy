#include "panel.h"

#include <math.h>
#include <stdio.h>

static const int whites[] = {0, 2, 4, 5, 7, 9, 11, 12};
static const int blacks[] = {1, 3, 6, 8, 10};
static const int black_slots[] = {1, 2, 4, 5, 6};
static const char *const bindings[] = {"A", "W", "S", "E", "D", "F", "T", "G", "Y", "H", "U", "J", "K"};
static const char *const notes[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"};
static const char *const labels[] = {"MORPH", "COLOR", "ECHO"};
static const char *const hints[] = {"OSCILLATOR BLEND", "FILTER CUTOFF", "STEREO DELAY"};

Layout panel_layout(bool touch, bool landscape)
{
    if (touch && landscape) {
        return (Layout){
            .width = 820, .height = 454, .touch = true,
            .scene = {16, 71, 474, 210}, .bank = {505, 71, 299, 65},
            .controls = {{505, 149, 93, 132}, {608, 149, 93, 132}, {711, 149, 93, 132}},
            .keys = {19, 328, 782, 91}, .previous = {513, 79, 47, 48}, .next = {749, 79, 47, 48}
        };
    }
    if (touch) {
        return (Layout){
            .width = 420, .height = 740, .touch = true,
            .scene = {16, 80, 388, 236}, .bank = {16, 329, 388, 72},
            .controls = {{16, 414, 124, 96}, {148, 414, 124, 96}, {280, 414, 124, 96}},
            .keys = {19, 555, 382, 143}, .previous = {26, 340, 48, 50}, .next = {346, 340, 48, 50}
        };
    }
    return (Layout){
        .width = 1000, .height = 740, .scene = {26, 107, 629, 332}, .bank = {673, 107, 301, 332},
        .controls = {{26, 457, 304, 103}, {348, 457, 304, 103}, {670, 457, 304, 103}},
        .keys = {39, 612, 922, 86}, .previous = {690, 388, 55, 35}, .next = {902, 388, 55, 35}
    };
}

Rect panel_key(const Layout *layout, int semitone)
{
    const Rect r = layout->keys;
    const float cell = r.w / GF_WHITE_COUNT;
    for (int i = 0; i < GF_WHITE_COUNT; ++i)
        if (semitone == whites[i]) return (Rect){r.x + i * cell, r.y, cell - 2, r.h};
    for (int i = 0; i < 5; ++i)
        if (semitone == blacks[i]) {
            const float w = cell * (layout->touch ? .65f : .57f);
            return (Rect){r.x + black_slots[i] * cell - w / 2, r.y, w, r.h * .62f};
        }
    return (Rect){0};
}

int panel_note(const Layout *layout, float x, float y)
{
    for (int i = 0; i < 5; ++i)
        if (inside(panel_key(layout, blacks[i]), x, y)) return blacks[i];
    for (int i = 0; i < GF_WHITE_COUNT; ++i)
        if (inside(panel_key(layout, whites[i]), x, y)) return whites[i];
    return -1;
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
    const float cx = r.x + r.w * .52f, cy = r.y + r.h * .50f;
    const float scale = r.h * .245f;
    const float orbit = r.h * .38f;
    const float bottom = r.y + r.h - 41;
    const float horizon = r.y + r.h * .75f;
    const Color grid = ink(0x263b2b), highlight = ink(0xcce77e);
    shade(r, ink(0x0b1517), ink(0x142720));
    for (int i = -8; i <= 8; ++i) {
        const float x = r.x + 10 + (i + 8) * (r.w - 20) / 16;
        line(cx + (x - cx) * .25f, horizon, x, bottom, 1, grid);
    }
    for (int i = 0; i < 7; ++i) {
        const float progress = i / 6.f;
        const float y = horizon + (bottom-horizon) * progress * progress;
        line(r.x + 10, y, r.x + r.w - 10, y, 1, grid);
    }
    for (unsigned i = 0; i < 55; ++i) {
        const unsigned hash = (i + 1) * 0x9e3779b9u;
        const float sx = r.x + 15 + (r.w - 30) * ((hash >> 8) & 1023) / 1024.f;
        const float sy = r.y + 42 + (r.h - 102) * ((hash >> 21) & 1023) / 1024.f;
        const float twinkle = .27f + .17f * sinf(v->time * 1.1f + i * 1.5f);
        const float size = i % 7 == 0 ? 11 : 2;
        sprite((Rect){sx, sy, size, size}, i % 7 == 0 ? 1 : 2, alpha(highlight, twinkle));
    }
    ellipse(cx, cy + 3, orbit, orbit * .33f, -.30f, ink(0x5b7448));
    ellipse(cx, cy, orbit * 1.27f, orbit * .43f, -.30f, ink(0x344537));
    ellipse(cx, cy + 3, orbit * .80f, orbit * .29f, .43f, ink(0x4b633c));
    const float floating = 5 * sinf(v->time * 1.2f);
    mesh_draw(v->mesh, cx, cy + floating, scale * (1 + v->peak * .13f),
              v->time * .48f, blend(ink(0x8eb861), highlight, .33f + v->peak * .45f));
    for (int i = 0; i < 3; ++i) {
        const float a = v->time * (.33f + .04f * i) + i * 2.0943951f;
        const float x = cx + orbit * (1.10f + i * .08f) * cosf(a);
        const float y = cy + orbit * (.36f * sinf(a) - .26f * cosf(a));
        mesh_draw(v->mesh, x, y, scale * (.11f + .025f * i), a * 1.75f, ink(0xa6bf73));
    }
    for (int i = 0; i < (int)r.h / 5; ++i)
        box((Rect){r.x + 2, r.y + i*5 + 2, r.w - 4, 1}, alpha(ink(0x07130f), .29f));
    frame(r, ink(0x455a45));
    const float pad = l->touch ? 12 : 17;
    sprite((Rect){r.x + pad, r.y + 16, 8, 8}, 2, highlight);
    text(r.x + pad + 15, r.y + 11, 15, ink(0xb5c6ab), "SIGNAL FIELD");
    text(r.x + r.w - pad - 59, r.y + 11, 13, ink(0x789485), "STEREO");
    box((Rect){r.x + 1, bottom, r.w - 2, 40}, ink(0x13211d));
    box((Rect){r.x + 1, bottom, r.w - 2, 1}, ink(0x334637));
    text(r.x + pad, bottom + 13, 13, ink(0xb0bd9d), "OUTPUT");
    const int bars = l->touch ? 15 : 24;
    const float step = l->touch ? 11 : 15;
    const float gain = sqrtf(fminf(1, v->peak));
    for (int i = 0; i < bars; ++i) {
        const bool lit = i + .4f < gain * bars;
        const Color c = lit ? ink(i > bars * .78f ? 0xeac078 : 0xc4dd75) : ink(0x2b3e34);
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

static void patches(const Layout *layout, const PanelView *v)
{
    const Rect r = layout->bank;
    const Color highlighted = ink(0xd4e986), pale = ink(0xe5ead2), muted = ink(0x9fb2a3);
    const char *name = v->bank->patches[v->selected].name;
    instrument_panel(r);
    button(layout->previous, "<");
    button(layout->next, ">");
    if (layout->touch) {
        center_text(r.x + 64, r.y + 9, r.w - 128, 11, muted, "PATCH");
        const float width = r.w - 135;
        const float size = fminf(23, 23 * width / text_width(23, name));
        center_text(r.x + 65, r.y + 26, r.w - 130, size, pale, name);
    } else {
        text(r.x + 17, r.y + 13, 14, highlighted, "PATCH BANK");
        fit_text(r.x + 17, r.y + 39, r.w - 34, 30, pale, name);
        box((Rect){r.x + 17, r.y + 82, r.w - 34, 1}, ink(0x3e5041));
        const size_t first = panel_first_patch(v->selected, v->bank->count);
        for (size_t row = 0; row < GF_VISIBLE_PATCHES && first + row < v->bank->count; ++row) {
            const size_t index = first + row;
            const Rect p = panel_patch_row(layout, row);
            const bool active = index == v->selected;
            box(p, ink(active ? 0x34482f : 0x14221d));
            frame(p, ink(active ? 0x667955 : 0x293a31));
            if (active) box((Rect){p.x, p.y, 3, p.h}, highlighted);
            char number[12];
            snprintf(number, sizeof(number), "%02d", (int)index + 1);
            text(p.x + 11, p.y + 8, 14, active ? highlighted : muted, number);
            fit_text(p.x + 44, p.y + 7, p.w - 53, 16, active ? pale : muted,
                     v->bank->patches[index].name);
        }
        char count[24];
        snprintf(count, sizeof(count), "%02d / %02d", (int)v->selected + 1, (int)v->bank->count);
        center_text(r.x, r.y + 229 + 65, r.w, 15, muted, count);
    }
}

static void parameters(const Layout *l, const PanelView *v)
{
    const Color accent = ink(0xd2e984);
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
        box((Rect){t.x, t.y, progress, t.h}, ink(0x8da958));
        box((Rect){t.x + progress - 4, t.y - 6, 8, t.h + 12}, accent);
    }
}

static void key(const Layout *l, const PanelView *v, int semitone, bool black)
{
    const Rect r = panel_key(l, semitone);
    const bool active = v->held[v->octave + semitone] > 0;
    const unsigned top = active ? 0xc3df78 : (black ? 0x334338 : 0xdce4ce);
    const unsigned base = active ? 0x9bb458 : (black ? 0x121f1b : 0xadbea8);
    shade(r, ink(top), ink(base));
    frame(r, ink(black ? 0x0e1b16 : 0x718572));
    box((Rect){r.x + 3, r.y + r.h - 4, r.w - 6, 2}, ink(black ? 0x526456 : 0x758b75));
    const float size = black && l->touch ? 11 : 13;
    const Color color = ink(black && !active ? 0xd5e1ca : 0x26392d);
    center_text(r.x, r.y + r.h - size - (l->touch ? 14 : 10), r.w, size, color,
                l->touch ? notes[semitone] : bindings[semitone]);
}

static void keyboard(const Layout *l, const PanelView *v)
{
    const Rect k = l->keys;
    const Rect shelf = {l->scene.x, k.y - 37, l->width - l->scene.x * 2, k.h + 46};
    instrument_panel(shelf);
    text(k.x + 1, k.y - 26, 14, ink(0xb8c9aa), l->touch ? "TOUCH KEYS" : "PLAY KEYS");
#ifdef __EMSCRIPTEN__
    const char *keys_help = "A-K TO PLAY   /   Z-X OCTAVE";
#else
    const char *keys_help = "A-K TO PLAY   /   Z-X OCTAVE   /   R RELOAD";
#endif
    const char *help = l->touch ? "TAP OR SLIDE" : keys_help;
    text(k.x + k.w - text_width(12, help), k.y - 25, 12, ink(0x9aafa1), help);
    for (int i = 0; i < GF_WHITE_COUNT; ++i) key(l, v, whites[i], false);
    for (int i = 0; i < 5; ++i) key(l, v, blacks[i], true);
    char octave[32];
    snprintf(octave, sizeof(octave), "C%d - C%d", v->octave / 12 - 1, v->octave / 12);
    const float right = l->width - l->scene.x;
    text(right - text_width(11, octave), l->height - 22, 11, ink(0x97ab98), octave);
    if (v->status && v->status[0])
        fit_text(l->scene.x, l->height - 23, l->width - l->scene.x * 2 - 68,
                 12, ink(0xe7cb86), v->status);
    else
        text(l->scene.x, l->height - 22, 11, ink(0x97ab98), "GREEN FAIRY  /  INSTRUMENT");
}

void panel_draw(const Layout *l, const PanelView *v)
{
    shade((Rect){0, 0, l->width, l->height}, ink(0x14231e), ink(0x0e1816));
    header(l, v);
    visualizer(l, v);
    patches(l, v);
    parameters(l, v);
    keyboard(l, v);
}
