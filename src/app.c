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
    Mesh prism, custom_mesh;
    const Mesh *scene_mesh;
    Layout layout;
    Contact touch[CONTACT_COUNT], mouse, xr[CONTACT_COUNT];
    unsigned held[GF_NOTES];
    int key_note[SDL_SCANCODE_COUNT];
    GF_Tuning custom_tuning, tuning_draft;
    int octave, tuning_index, tuning_focus;
    TuningOverlay tuning_overlay;
    char tuning_path[4096];
    size_t selected;
    float controls[GF_CONTROL_COUNT];
    float *sample;
    float zoom, offset_x, offset_y;
    float peak;
    size_t voices;
    Uint64 scene_started;
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

static const GF_Tuning *active_tuning(void)
{
    return app.tuning_index == GF_CUSTOM_TUNING ? &app.custom_tuning : &gf_tunings[app.tuning_index];
}

#ifdef __EMSCRIPTEN__
EM_JS(void, save_web_tuning, (int index, int octave, int root, unsigned degrees), {
    try {
        localStorage.setItem("green-fairy.tuning.v1", JSON.stringify([index, octave, root, degrees]));
    } catch (_) {}
})
#endif

static void save_tuning(void)
{
#ifdef __EMSCRIPTEN__
    save_web_tuning(app.tuning_index, app.octave, app.custom_tuning.root, app.custom_tuning.degrees);
#else
    if (!app.tuning_path[0]) return;
    char data[80];
    const int length = SDL_snprintf(data, sizeof(data), "%d %d %d %lu\n",
        app.tuning_index, app.octave, app.custom_tuning.root, (unsigned long)app.custom_tuning.degrees);
    if (!SDL_SaveFile(app.tuning_path, data, (size_t)length)) SDL_Log("Save tuning: %s", SDL_GetError());
#endif
}

static void restore_tuning(int index, int octave, int root, unsigned degrees)
{
    const GF_Tuning custom = {degrees, root};
    if (index < 0 || index >= GF_TUNING_COUNT || !gf_tuning_valid(&custom)) return;
    app.custom_tuning = custom;
    app.tuning_index = index;
    app.octave = gf_tuning_octave(active_tuning(), octave);
}

static void init_tuning(bool smoke)
{
    app.custom_tuning = gf_tunings[0];
    app.octave = 2;
    if (smoke) return;
#ifndef __EMSCRIPTEN__
    char *path = SDL_GetPrefPath("Green Fairy", "Instrument");
    if (!path) return;
    const int size = SDL_snprintf(app.tuning_path, sizeof(app.tuning_path), "%stuning.txt", path);
    SDL_free(path);
    if (size < 0 || (size_t)size >= sizeof(app.tuning_path)) {
        app.tuning_path[0] = 0;
        return;
    }
    char *data = SDL_LoadFile(app.tuning_path, NULL);
    if (!data) return;
    int index, octave, root;
    unsigned degrees;
    if (sscanf(data, "%d%d%d%u", &index, &octave, &root, &degrees) == 4)
        restore_tuning(index, octave, root, degrees);
    SDL_free(data);
#endif
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

static void start_note(int note, float velocity)
{
    if (app.held[note]++ == 0) {
        audio_lock();
        const float pitch = gf_tuning_cents(active_tuning(), app.octave, note) / 100.f;
        gf_note_on_pitch(&app.synth, note, pitch, velocity);
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

static void select_tuning(int index)
{
    release_input();
    app.tuning_index = index;
    app.octave = gf_tuning_octave(active_tuning(), app.octave);
    app.tuning_overlay = GF_TUNING_CLOSED;
    save_tuning();
}

static void shift_octave(int direction)
{
    const int octave = gf_tuning_octave(active_tuning(), app.octave + direction);
    if (octave == app.octave) return;
    release_input();
    app.octave = octave;
    save_tuning();
}

static void open_builder(void)
{
    release_input();
    app.tuning_draft = *active_tuning();
    app.tuning_focus = 1;
    app.tuning_overlay = GF_TUNING_BUILDER;
}

static void apply_builder(void)
{
    if (!gf_tuning_valid(&app.tuning_draft)) return;
    app.custom_tuning = app.tuning_draft;
    select_tuning(GF_CUSTOM_TUNING);
}

static void shift_root(int direction)
{
    app.tuning_draft.root = (app.tuning_draft.root + 12 + direction) % 12;
}

static void toggle_degree(int index)
{
    if (index > 0) app.tuning_draft.degrees ^= UINT32_C(1) << index;
}

static bool tuning_press(const Layout *layout, float x, float y)
{
    if (app.tuning_overlay == GF_TUNING_MENU) {
        app.tuning_overlay = GF_TUNING_CLOSED;
        for (int i = 0; i < GF_TUNING_COUNT; ++i) {
            if (inside(panel_tuning_option(layout, i), x, y)) {
                select_tuning(i);
                break;
            }
        }
        return true;
    }
    if (app.tuning_overlay == GF_TUNING_BUILDER) {
        const TuningBuilder b = panel_tuning_builder(layout);
        if (inside(b.cancel, x, y)) app.tuning_overlay = GF_TUNING_CLOSED;
        else if (inside(b.apply, x, y)) apply_builder();
        else if (inside(b.lower, x, y)) shift_root(-1);
        else if (inside(b.raise, x, y)) shift_root(1);
        else for (int i = 0; i < GF_DEGREES; ++i) {
            if (!inside(panel_tuning_degree(&b, i), x, y)) continue;
            app.tuning_focus = i;
            toggle_degree(i);
            break;
        }
        return true;
    }
    const TuningBar bar = panel_tuning_bar(layout);
    if (inside(bar.select, x, y)) {
        release_input();
        app.tuning_focus = app.tuning_index;
        app.tuning_overlay = GF_TUNING_MENU;
    } else if (inside(bar.build, x, y)) open_builder();
    else if (inside(bar.lower, x, y)) shift_octave(-1);
    else if (inside(bar.raise, x, y)) shift_octave(1);
    else return false;
    return true;
}

static void select_patch(size_t index)
{
    release_input();
    app.selected = index;
    const GF_Patch *patch = &app.bank.patches[index];
    const GF_Scene *scene = &app.bank.scenes[index];
    app.scene_started = SDL_GetTicks();
    app.scene_mesh = &app.prism;
    bool valid_scene = true;
    if (strcmp(scene->mesh, GF_DEFAULT_MESH) != 0) {
        char path[4096];
        asset_path(path, sizeof(path), "graphics", scene->mesh);
        valid_scene = mesh_load(&app.custom_mesh, path);
        if (valid_scene) app.scene_mesh = &app.custom_mesh;
    }
    size_t length = 0;
    float *sample = load_sample(patch->sample_file, &length);
    if (patch->sample_file[0] && !sample) {
        char message[80];
        SDL_snprintf(message, sizeof(message), "Could not load WAV: %s", patch->sample_file);
        status(message);
    } else if (!valid_scene) {
        char message[80];
        SDL_snprintf(message, sizeof(message), "Using prism fallback for %s", scene->mesh);
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
    const int note = panel_note(layout, x, y);
    if (note == contact->note) return;
    end_note(contact->note);
    contact->note = note;
    if (note >= 0) start_note(note, .86f);
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
    if (tuning_press(layout, x, y)) return;
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

static int letter_slot(const SDL_KeyboardEvent *event)
{
    SDL_Keycode code = event->key;
    const SDL_Scancode scan = event->scancode;
    if (scan >= SDL_SCANCODE_1 && scan <= SDL_SCANCODE_0)
        return GF_PLAY_KEYS - 10 + scan - SDL_SCANCODE_1;
    if (scan >= SDL_SCANCODE_KP_1 && scan <= SDL_SCANCODE_KP_0)
        return GF_PLAY_KEYS - 10 + scan - SDL_SCANCODE_KP_1;
    if (code == SDLK_UNKNOWN && scan >= SDL_SCANCODE_A && scan <= SDL_SCANCODE_Z)
        code = SDLK_A + scan - SDL_SCANCODE_A;
    if (code >= SDLK_A && code <= SDLK_Z) code = 'A' + code - SDLK_A;
    else if (code < SDLK_0 || code > SDLK_9) return -1;
    const char *binding = strchr(gf_play_keys, (int)code);
    return binding ? (int)(binding - gf_play_keys) : -1;
}

static void tuning_keyboard(SDL_Scancode scancode)
{
    if (scancode == SDL_SCANCODE_ESCAPE) {
        app.tuning_overlay = GF_TUNING_CLOSED;
        return;
    }
    if (app.tuning_overlay == GF_TUNING_MENU) {
        if (scancode == SDL_SCANCODE_RETURN) select_tuning(app.tuning_focus);
        else if (scancode == SDL_SCANCODE_UP)
            app.tuning_focus = (app.tuning_focus + GF_TUNING_COUNT - 1) % GF_TUNING_COUNT;
        else if (scancode == SDL_SCANCODE_DOWN) app.tuning_focus = (app.tuning_focus + 1) % GF_TUNING_COUNT;
        return;
    }
    int offset = 0;
    switch (scancode) {
    case SDL_SCANCODE_LEFT: offset = -1; break;
    case SDL_SCANCODE_RIGHT: offset = 1; break;
    case SDL_SCANCODE_UP: offset = -GF_BUILDER_COLUMNS; break;
    case SDL_SCANCODE_DOWN: offset = GF_BUILDER_COLUMNS; break;
    case SDL_SCANCODE_LEFTBRACKET: shift_root(-1); break;
    case SDL_SCANCODE_RIGHTBRACKET: shift_root(1); break;
    case SDL_SCANCODE_SPACE: toggle_degree(app.tuning_focus); break;
    case SDL_SCANCODE_RETURN: apply_builder(); break;
    default: break;
    }
    app.tuning_focus = (app.tuning_focus + GF_DEGREES + offset) % GF_DEGREES;
}

static void keyboard_event(const SDL_KeyboardEvent *event)
{
    if (event->scancode <= SDL_SCANCODE_UNKNOWN || event->scancode >= SDL_SCANCODE_COUNT) return;
    int *note = &app.key_note[event->scancode];
    if (!event->down) {
        end_note(*note);
        *note = -1;
        return;
    }
    if (event->repeat || (event->mod & (SDL_KMOD_CTRL | SDL_KMOD_ALT | SDL_KMOD_GUI))) return;
    if (app.tuning_overlay != GF_TUNING_CLOSED) {
        tuning_keyboard(event->scancode);
        return;
    }
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
#ifndef __EMSCRIPTEN__
    case SDL_SCANCODE_F5: reload_bank(); return;
#endif
    case SDL_SCANCODE_LEFT: shift_octave(-1); return;
    case SDL_SCANCODE_RIGHT: shift_octave(1); return;
    case SDL_SCANCODE_RETURN:
        release_input();
        app.tuning_focus = app.tuning_index;
        app.tuning_overlay = GF_TUNING_MENU;
        return;
    case SDL_SCANCODE_SPACE: open_builder(); return;
    default: break;
    }
    if (*note < 0) {
        *note = letter_slot(event);
        if (*note >= 0) start_note(*note, (event->mod & SDL_KMOD_SHIFT) ? 1.f : .86f);
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
        .bank = &app.bank, .mesh = app.scene_mesh, .prism = &app.prism,
        .selected = app.selected, .octave = app.octave, .tuning = active_tuning(),
        .draft = &app.tuning_draft, .tuning_index = app.tuning_index,
        .tuning_focus = app.tuning_focus, .tuning_overlay = app.tuning_overlay,
        .voices = app.voices, .held = app.held, .controls = app.controls, .peak = app.peak,
        .time = (SDL_GetTicks() - app.scene_started) / 1000.f, .status = app.status
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

static bool initialize(bool smoke)
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
    asset_path(path, sizeof(path), "graphics", GF_DEFAULT_MESH);
    if (!mesh_load(&app.prism, path)) return SDL_SetError("Invalid prism mesh: %s", path);

    gf_init(&app.synth, AUDIO_RATE);
    if (use_audio) {
        const SDL_AudioSpec format = {.format = SDL_AUDIO_F32, .channels = 2, .freq = AUDIO_RATE};
        app.audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &format,
                                              audio_callback, &app.synth);
        if (app.audio) SDL_ResumeAudioStreamDevice(app.audio);
    }
    init_tuning(smoke);
    for (int i = 0; i < SDL_SCANCODE_COUNT; ++i) app.key_note[i] = -1;
    if (!reload_bank()) {
        app.bank.patches[0] = gf_default_patch();
        app.bank.scenes[0] = gf_default_scene();
        app.bank.count = 1;
        app.scene_mesh = &app.prism;
        app.scene_started = SDL_GetTicks();
    }
    resize();
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
EMSCRIPTEN_KEEPALIVE int gf_web_tuning(void) { return app.tuning_index; }
EMSCRIPTEN_KEEPALIVE int gf_web_tuning_overlay(void) { return app.tuning_overlay; }
EMSCRIPTEN_KEEPALIVE int gf_web_tuning_cents(int key)
{
    return key >= 0 && key < GF_PLAY_KEYS ? gf_tuning_cents(active_tuning(), app.octave, key) : -1;
}
EMSCRIPTEN_KEEPALIVE int gf_web_key_held(int key)
{
    return key >= 0 && key < GF_PLAY_KEYS && app.held[key] > 0;
}
EMSCRIPTEN_KEEPALIVE void gf_web_restore_tuning(int index, int octave, int root, unsigned degrees)
{
    release_input();
    restore_tuning(index, octave, root, degrees);
}

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
    if (app.bank.count != GF_PATCHES || !app.sample || !app.audio || !app.prism.face_count) return false;
    for (size_t i = 0; i < app.bank.count; ++i) {
        select_patch(i);
        if (app.status[0] || !app.scene_mesh->face_count) return false;
    }
    select_patch(0);
    SDL_Event press = {0};
    press.key.type = SDL_EVENT_KEY_DOWN;
    press.key.down = true;
    press.key.scancode = SDL_SCANCODE_A;
    if (!SDL_PushEvent(&press)) return false;
    const Uint64 deadline = SDL_GetTicks() + 2500;
    do {
        frame_loop();
        if (app.held[7] && app.peak > .008f) {
            press.key.type = SDL_EVENT_KEY_UP;
            press.key.down = false;
            if (!SDL_PushEvent(&press)) return false;
            frame_loop();
            if (app.held[7]) return false;
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
    const bool smoke = argc > 1 && strcmp(argv[1], "--smoke") == 0;
    if (!initialize(smoke)) {
        SDL_Log("Green Fairy startup failed: %s", SDL_GetError());
        cleanup();
        return 1;
    }
#ifdef __EMSCRIPTEN__
    (void)argc;
    (void)argv;
    emscripten_set_main_loop(frame_loop, 0, true);
#else
    if (smoke) {
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
