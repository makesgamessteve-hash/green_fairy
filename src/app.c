#include "panel.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

enum { CONTACT_COUNT = 16, AUDIO_RATE = 48000, AUDIO_BLOCK = 512 };

typedef struct {
    Uint64 id;
    int note, control;
    bool active;
} Contact;

typedef struct {
    SDL_Window *window;
    SDL_GLContext gl;
    SDL_AudioStream *audio;
    GF_Synth synth;
    GF_Bank bank;
    Mesh mesh;
    Layout layout;
    Contact touch[CONTACT_COUNT], mouse, xr[CONTACT_COUNT];
    unsigned held[GF_NOTES];
    int key_note[SDL_SCANCODE_COUNT];
    int octave;
    size_t selected;
    float controls[GF_CONTROL_COUNT];
    float *sample;
    float zoom, offset_x, offset_y;
    float peak;
    size_t voices;
    Uint64 started;
    const char *base;
    char status[256];
    bool ready, running, coarse, immersed, vsync, graphics_suspended;
} App;

static App app;

static void asset_path(char *out, size_t capacity, const char *folder, const char *name)
{
    SDL_snprintf(out, capacity, "%sassets/%s/%s", app.base, folder, name);
}

static void SDLCALL audio_callback(void *data, SDL_AudioStream *stream, int additional, int total)
{
    (void)total;
    GF_Synth *synth = data;
    float buffer[AUDIO_BLOCK * 2];
    const int frame_bytes = (int)(2 * sizeof(float));
    while (additional > 0) {
        const int frames = SDL_min(AUDIO_BLOCK, (additional + frame_bytes - 1) / frame_bytes);
        gf_render(synth, buffer, frames);
        const int bytes = frames * frame_bytes;
        if (!SDL_PutAudioStreamData(stream, buffer, bytes)) break;
        additional -= bytes;
    }
}

static void audio_lock(void)
{
    if (app.audio) SDL_LockAudioStream(app.audio);
}

static void audio_unlock(void)
{
    if (app.audio) SDL_UnlockAudioStream(app.audio);
}

static void status(const char *message)
{
    SDL_strlcpy(app.status, message, sizeof(app.status));
    if (*message) SDL_Log("%s", message);
}

static float *load_sample(const char *name, size_t *frames)
{
    if (!name[0]) return NULL;
    char path[4096];
    asset_path(path, sizeof(path), "sounds", name);
    SDL_AudioSpec input;
    Uint8 *data;
    Uint32 length;
    if (!SDL_LoadWAV(path, &input, &data, &length)) return NULL;
    const SDL_AudioSpec output = {.format = SDL_AUDIO_F32, .channels = 1, .freq = AUDIO_RATE};
    Uint8 *converted = NULL;
    int bytes = 0;
    if (length > 16 * 1024 * 1024 ||
        !SDL_ConvertAudioSamples(&input, data, (int)length, &output, &converted, &bytes)) {
        SDL_free(data);
        return NULL;
    }
    SDL_free(data);
    *frames = (size_t)bytes / sizeof(float);
    return (float *)converted;
}

static void end_note(int note)
{
    if (note < 0 || app.held[note] == 0) return;
    if (--app.held[note] == 0) {
        audio_lock();
        gf_note_off(&app.synth, note);
        audio_unlock();
    }
}

static void start_note(int note)
{
    if (app.held[note]++ == 0) {
        audio_lock();
        gf_note_on(&app.synth, note, .86f);
        audio_unlock();
    }
}

static void contact_end(Contact *contact)
{
    if (!contact->active) return;
    end_note(contact->note);
    contact->active = false;
}

static void release_input(void)
{
    contact_end(&app.mouse);
    for (int i = 0; i < CONTACT_COUNT; ++i) {
        contact_end(&app.touch[i]);
        contact_end(&app.xr[i]);
    }
    for (int i = 0; i < SDL_SCANCODE_COUNT; ++i) {
        end_note(app.key_note[i]);
        app.key_note[i] = -1;
    }
}

static void select_patch(size_t index)
{
    release_input();
    app.selected = index;
    const GF_Patch *patch = &app.bank.patches[index];
    size_t length = 0;
    float *sample = load_sample(patch->sample_file, &length);
    if (patch->sample_file[0] && !sample) {
        char message[80];
        SDL_snprintf(message, sizeof(message), "Could not load WAV: %s", patch->sample_file);
        status(message);
    } else if (app.audio) status("");
    else status("Audio device unavailable");
    audio_lock();
    gf_set_patch(&app.synth, patch);
    gf_set_sample(&app.synth, sample, length);
    audio_unlock();
    SDL_free(app.sample);
    app.sample = sample;
    app.controls[GF_MORPH] = patch->morph;
    app.controls[GF_COLOR] = patch->color;
    app.controls[GF_ECHO] = patch->echo;
}

static bool reload_bank(void)
{
    char path[4096], error[256];
    asset_path(path, sizeof(path), "scripts", "bank.lua");
    size_t length;
    char *source = SDL_LoadFile(path, &length);
    if (!source) {
        status(SDL_GetError());
        return false;
    }
    const bool valid = gf_bank_parse(source, length, &app.bank, error, sizeof(error)) != 0;
    SDL_free(source);
    if (!valid) {
        status(error);
        return false;
    }
    select_patch(app.selected < app.bank.count ? app.selected : 0);
    return true;
}

static void step_patch(int step)
{
    const int count = (int)app.bank.count;
    select_patch((size_t)(((int)app.selected + step % count + count) % count));
}

static void update_control(const Layout *layout, int control, float x)
{
    const Rect track = panel_track(layout, control);
    const float value = SDL_clamp((x - track.x) / track.w, 0, 1);
    app.controls[control] = value;
    audio_lock();
    gf_control(&app.synth, (GF_Control)control, value);
    audio_unlock();
}

static void update_note(Contact *contact, const Layout *layout, float x, float y)
{
    const int key = panel_note(layout, x, y);
    const int note = key < 0 ? -1 : app.octave + key;
    if (note == contact->note) return;
    end_note(contact->note);
    contact->note = note;
    if (note >= 0) start_note(note);
}

static void pointer(Contact *contact, const Layout *layout, int phase, float x, float y)
{
    if (phase == 2) {
        contact_end(contact);
        return;
    }
    if (phase == 1) {
        if (!contact->active) return;
        if (contact->control >= 0) update_control(layout, contact->control, x);
        else update_note(contact, layout, x, y);
        return;
    }
    const struct { Rect bounds; int step; } navigation[] = {
        {layout->jump_back, -GF_PATCH_JUMP}, {layout->previous, -1},
        {layout->next, 1}, {layout->jump_forward, GF_PATCH_JUMP}
    };
    for (size_t i = 0; i < SDL_arraysize(navigation); ++i) {
        if (inside(navigation[i].bounds, x, y)) {
            step_patch(navigation[i].step);
            return;
        }
    }
    if (!layout->touch && inside(layout->bank, x, y)) {
        const size_t first = panel_first_patch(app.selected, app.bank.count);
        for (size_t i = 0; i < GF_VISIBLE_PATCHES && first + i < app.bank.count; ++i)
            if (inside(panel_patch_row(layout, i), x, y)) select_patch(first + i);
        return;
    }
    contact_end(contact);
    *contact = (Contact){.id = contact->id, .note = -1, .control = -1, .active = true};
    for (int i = 0; i < GF_CONTROL_COUNT; ++i) {
        if (inside(layout->controls[i], x, y)) {
            contact->control = i;
            update_control(layout, i, x);
            return;
        }
    }
    update_note(contact, layout, x, y);
}

static void window_pointer(Contact *contact, int phase, float x, float y)
{
    pointer(contact, &app.layout, phase,
            (x - app.offset_x) / app.zoom, (y - app.offset_y) / app.zoom);
}

static Contact *touch_contact(Uint64 id, bool begin)
{
    Contact *free_slot = NULL;
    for (int i = 0; i < CONTACT_COUNT; ++i) {
        if (app.touch[i].active && app.touch[i].id == id) return &app.touch[i];
        if (!app.touch[i].active && !free_slot) free_slot = &app.touch[i];
    }
    if (!begin || !free_slot) return NULL;
    free_slot->id = id;
    return free_slot;
}

static void resize(void)
{
    int width = 1, height = 1;
    SDL_GetWindowSize(app.window, &width, &height);
    app.layout = panel_layout(app.coarse || width < 570, width > height * 1.3f);
    app.zoom = fmaxf(.001f, fminf(width / app.layout.width, height / app.layout.height));
    app.offset_x = (width - app.layout.width * app.zoom) / 2;
    app.offset_y = (height - app.layout.height * app.zoom) / 2;
}

static void keyboard_event(const SDL_KeyboardEvent *event)
{
    static const SDL_Scancode keys[] = {
        SDL_SCANCODE_A, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_E, SDL_SCANCODE_D,
        SDL_SCANCODE_F, SDL_SCANCODE_T, SDL_SCANCODE_G, SDL_SCANCODE_Y, SDL_SCANCODE_H,
        SDL_SCANCODE_U, SDL_SCANCODE_J, SDL_SCANCODE_K
    };
    if (event->scancode <= SDL_SCANCODE_UNKNOWN || event->scancode >= SDL_SCANCODE_COUNT) return;
    int *note = &app.key_note[event->scancode];
    if (!event->down) {
        end_note(*note);
        *note = -1;
        return;
    }
    if (event->repeat) return;
    switch (event->scancode) {
    case SDL_SCANCODE_ESCAPE:
#ifndef __EMSCRIPTEN__
        app.running = false;
#else
        release_input();
#endif
        return;
    case SDL_SCANCODE_UP: step_patch(-1); return;
    case SDL_SCANCODE_DOWN: step_patch(1); return;
    case SDL_SCANCODE_PAGEUP: step_patch(-GF_PATCH_JUMP); return;
    case SDL_SCANCODE_PAGEDOWN: step_patch(GF_PATCH_JUMP); return;
    case SDL_SCANCODE_R:
#ifndef __EMSCRIPTEN__
        reload_bank();
#else
        status("Refresh the page to reload assets");
#endif
        return;
    case SDL_SCANCODE_Z:
    case SDL_SCANCODE_X:
        release_input();
        app.octave = SDL_clamp(app.octave + (event->scancode == SDL_SCANCODE_X ? 12 : -12), 24, 84);
        return;
    default: break;
    }
    for (int i = 0; i < GF_KEY_COUNT; ++i)
        if (event->scancode == keys[i] && *note < 0) {
            *note = app.octave + i;
            start_note(*note);
            break;
        }
}

static void events(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: app.running = false; break;
        case SDL_EVENT_WINDOW_RESIZED: resize(); break;
        case SDL_EVENT_WINDOW_FOCUS_LOST: release_input(); break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: keyboard_event(&event.key); break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.which != SDL_TOUCH_MOUSEID && event.button.button == SDL_BUTTON_LEFT)
                window_pointer(&app.mouse, event.button.down ? 0 : 2, event.button.x, event.button.y);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (event.motion.which != SDL_TOUCH_MOUSEID)
                window_pointer(&app.mouse, 1, event.motion.x, event.motion.y);
            break;
        case SDL_EVENT_FINGER_DOWN:
        case SDL_EVENT_FINGER_MOTION:
        case SDL_EVENT_FINGER_UP:
        case SDL_EVENT_FINGER_CANCELED: {
            const int phase = event.type == SDL_EVENT_FINGER_DOWN ? 0 :
                              event.type == SDL_EVENT_FINGER_MOTION ? 1 : 2;
            Contact *contact = touch_contact(event.tfinger.fingerID, phase == 0);
            if (contact) {
                int width, height;
                SDL_GetWindowSize(app.window, &width, &height);
                window_pointer(contact, phase, event.tfinger.x * width, event.tfinger.y * height);
            }
            break;
        }
        default: break;
        }
    }
}

static PanelView view(void)
{
    audio_lock();
    app.peak = app.synth.peak;
    app.voices = gf_active_voices(&app.synth);
    audio_unlock();
    return (PanelView){
        .bank = &app.bank, .mesh = &app.mesh, .selected = app.selected, .octave = app.octave,
        .voices = app.voices, .held = app.held, .controls = app.controls, .peak = app.peak,
        .time = (SDL_GetTicks() - app.started) / 1000.f, .status = app.status
    };
}

static void render_window(void)
{
    int logical_w, logical_h, pixel_w, pixel_h;
    SDL_GetWindowSize(app.window, &logical_w, &logical_h);
    SDL_GetWindowSizeInPixels(app.window, &pixel_w, &pixel_h);
    if (logical_w <= 0 || logical_h <= 0 || pixel_w <= 0 || pixel_h <= 0) return;
    const float matrix[16] = {
        2 * app.zoom / logical_w, 0, 0, 0,
        0, -2 * app.zoom / logical_h, 0, 0, 0, 0, 1, 0,
        2 * app.offset_x / logical_w - 1, 1 - 2 * app.offset_y / logical_h, 0, 1
    };
    const PanelView state = view();
    draw_begin();
    panel_draw(&app.layout, &state);
    draw_end(matrix, 0, 0, pixel_w, pixel_h, ink(0x0e1916));
    SDL_GL_SwapWindow(app.window);
}

static void frame_loop(void)
{
    events();
    if (app.running && !app.immersed && !app.graphics_suspended) render_window();
}

static bool initialize(void)
{
    const bool use_audio = SDL_Init(SDL_INIT_AUDIO);
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return false;
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
#ifdef __EMSCRIPTEN__
    app.base = "/";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    app.base = SDL_GetBasePath();
    if (!app.base) return false;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    app.window = SDL_CreateWindow("Green Fairy", 1100, 814,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!app.window) return false;
    app.gl = SDL_GL_CreateContext(app.window);
    if (!app.gl) return false;
    app.vsync = SDL_GL_SetSwapInterval(1);

    char path[4096];
    asset_path(path, sizeof(path), "graphics", "studio.bmp");
    if (!draw_init(path)) return false;
    asset_path(path, sizeof(path), "graphics", "prism.obj");
    if (!mesh_load(&app.mesh, path)) return SDL_SetError("Invalid prism mesh: %s", path);

    gf_init(&app.synth, AUDIO_RATE);
    if (use_audio) {
        const SDL_AudioSpec format = {.format = SDL_AUDIO_F32, .channels = 2, .freq = AUDIO_RATE};
        app.audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &format,
                                              audio_callback, &app.synth);
        if (app.audio) SDL_ResumeAudioStreamDevice(app.audio);
    }
    app.octave = 60;
    for (int i = 0; i < SDL_SCANCODE_COUNT; ++i) app.key_note[i] = -1;
    if (!reload_bank()) {
        app.bank.patches[0] = gf_default_patch();
        app.bank.count = 1;
    }
    resize();
    app.started = SDL_GetTicks();
    app.ready = app.running = true;
    return true;
}

static void cleanup(void)
{
    app.ready = false;
    if (app.audio) SDL_DestroyAudioStream(app.audio);
    SDL_free(app.sample);
    if (app.gl) {
        draw_destroy();
        SDL_GL_DestroyContext(app.gl);
    }
    if (app.window) SDL_DestroyWindow(app.window);
    SDL_Quit();
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE void gf_web_context_lost(void)
{
    app.graphics_suspended = true;
    draw_forget_context();
}

EMSCRIPTEN_KEEPALIVE int gf_web_context_restored(void)
{
    if (!app.ready) return 0;
    char path[4096];
    asset_path(path, sizeof(path), "graphics", "studio.bmp");
    app.graphics_suspended = !draw_init(path);
    return !app.graphics_suspended;
}

EMSCRIPTEN_KEEPALIVE void gf_web_resize(int width, int height, int touch)
{
    if (!app.ready || width <= 0 || height <= 0) return;
    app.coarse = touch != 0;
    SDL_SetWindowSize(app.window, width, height);
    resize();
}

EMSCRIPTEN_KEEPALIVE void gf_web_release(void)
{
    if (app.ready) release_input();
}

EMSCRIPTEN_KEEPALIVE int gf_web_ready(void) { return app.ready; }
EMSCRIPTEN_KEEPALIVE int gf_web_touch(void) { return app.layout.touch; }
EMSCRIPTEN_KEEPALIVE int gf_web_patch(void) { return (int)app.selected; }
EMSCRIPTEN_KEEPALIVE int gf_web_held(void)
{
    int count = 0;
    for (int i = 0; i < GF_NOTES; ++i) count += app.held[i] > 0;
    return count;
}
EMSCRIPTEN_KEEPALIVE float gf_web_peak(void) { return app.peak; }
EMSCRIPTEN_KEEPALIVE float gf_web_control(int control)
{
    return control >= 0 && control < GF_CONTROL_COUNT ? app.controls[control] : 0;
}

static struct { float x, y; bool visible; } xr_cursor[CONTACT_COUNT];

EMSCRIPTEN_KEEPALIVE void gf_xr_cursor(int contact, float x, float y, int visible)
{
    if (contact < 0 || contact >= CONTACT_COUNT) return;
    xr_cursor[contact].x = x;
    xr_cursor[contact].y = y;
    xr_cursor[contact].visible = visible != 0;
}

EMSCRIPTEN_KEEPALIVE void gf_xr_mode(int enabled)
{
    release_input();
    app.immersed = enabled != 0;
    memset(xr_cursor, 0, sizeof(xr_cursor));
}

EMSCRIPTEN_KEEPALIVE void gf_xr_pointer(int contact, int phase, float x, float y)
{
    if (contact < 0 || contact >= CONTACT_COUNT || !app.immersed) return;
    const Layout layout = panel_layout(false, false);
    pointer(&app.xr[contact], &layout, phase, x * layout.width, y * layout.height);
}

static float xr_matrix[16];
EMSCRIPTEN_KEEPALIVE float *gf_xr_matrix(void) { return xr_matrix; }

EMSCRIPTEN_KEEPALIVE void gf_xr_draw(int x, int y, int width, int height)
{
    if (!app.immersed || app.graphics_suspended || width <= 0 || height <= 0) return;
    const Layout layout = panel_layout(false, false);
    const PanelView state = view();
    draw_begin();
    panel_draw(&layout, &state);
    for (int i = 0; i < CONTACT_COUNT; ++i) {
        if (!xr_cursor[i].visible) continue;
        const float cx = xr_cursor[i].x * layout.width;
        const float cy = xr_cursor[i].y * layout.height;
        const Color reticle = ink(0xf6d695);
        frame((Rect){cx - 9, cy - 9, 18, 18}, reticle);
        box((Rect){cx - 2, cy - 2, 4, 4}, reticle);
    }
    draw_end(xr_matrix, x, y, width, height, ink(0x081310));
}
#else
static bool smoke_test(void)
{
    if (app.bank.count != GF_PATCHES || !app.sample || !app.audio || app.mesh.face_count == 0) return false;
    SDL_Event press = {0};
    press.key.type = SDL_EVENT_KEY_DOWN;
    press.key.down = true;
    press.key.scancode = SDL_SCANCODE_A;
    if (!SDL_PushEvent(&press)) return false;
    const Uint64 deadline = SDL_GetTicks() + 2500;
    do {
        frame_loop();
        if (app.held[60] && app.peak > .008f) {
            press.key.type = SDL_EVENT_KEY_UP;
            press.key.down = false;
            if (!SDL_PushEvent(&press)) return false;
            frame_loop();
            if (app.held[60]) return false;
            step_patch(-1);
            if (app.selected != GF_PATCHES - 1) return false;
            step_patch(1);
            if (app.selected != 0 || !app.sample) return false;
            step_patch(1);
            return app.selected == 1 && app.sample == NULL;
        }
        SDL_Delay(8);
    } while (SDL_GetTicks() < deadline);
    return false;
}
#endif

int main(int argc, char **argv)
{
    if (!initialize()) {
        SDL_Log("Green Fairy startup failed: %s", SDL_GetError());
        cleanup();
        return 1;
    }
#ifdef __EMSCRIPTEN__
    (void)argc;
    (void)argv;
    emscripten_set_main_loop(frame_loop, 0, true);
#else
    if (argc > 1 && strcmp(argv[1], "--smoke") == 0) {
        const bool passed = smoke_test();
        if (!passed) SDL_Log("Green Fairy smoke test failed");
        cleanup();
        return passed ? 0 : 1;
    }
    while (app.running) {
        frame_loop();
        if (!app.vsync) SDL_Delay(8);
    }
    cleanup();
#endif
    return 0;
}
