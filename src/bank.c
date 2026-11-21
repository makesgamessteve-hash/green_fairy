#include "synth.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void limit(lua_State *state, lua_Debug *record)
{
    (void)record;
    luaL_error(state, "patch bank exceeded its Lua instruction budget");
}

static int report(char *error, size_t size, const char *message)
{
    if (error && size) snprintf(error, size, "%s", message ? message : "Lua bank failed");
    return 0;
}

static void string_field(lua_State *state, const char *key, char *out, size_t size)
{
    lua_getfield(state, -1, key);
    if (lua_type(state, -1) != LUA_TNIL) {
        size_t length = 0;
        const char *value = lua_type(state, -1) == LUA_TSTRING ?
            lua_tolstring(state, -1, &length) : NULL;
        if (!value || !length || length >= size || strlen(value) != length)
            luaL_error(state, "%s must be nonempty text shorter than %d bytes", key, (int)size);
        memcpy(out, value, length + 1);
    }
    lua_pop(state, 1);
}

static float field(lua_State *state, const char *key, float current, float min, float max)
{
    lua_getfield(state, -1, key);
    if (lua_type(state, -1) != LUA_TNIL) {
        if (lua_type(state, -1) != LUA_TNUMBER)
            luaL_error(state, "%s must be numeric", key);
        const lua_Number number = lua_tonumber(state, -1);
        if (!isfinite(number) || number < min || number > max)
            luaL_error(state, "%s is outside its allowed range", key);
        current = (float)number;
    }
    lua_pop(state, 1);
    return current;
}

static int choice(lua_State *state, const char *key, const char *const *names,
                  size_t count, int current)
{
    char name[GF_NAME] = {0};
    string_field(state, key, name, sizeof(name));
    if (!name[0] && current >= 0) return current;
    for (size_t i = 0; i < count; ++i)
        if (strcmp(name, names[i]) == 0) return (int)i;
    return luaL_error(state, "unrecognized value for %s", key);
}

static GF_Wave wave(lua_State *state, const char *key, GF_Wave current)
{
    static const char *const names[] = {"sine", "triangle", "saw", "square", "noise"};
    return (GF_Wave)choice(state, key, names, sizeof(names) / sizeof(names[0]), current);
}

static int optional_table(lua_State *state, const char *key)
{
    if (lua_getfield(state, -1, key) == LUA_TNIL) {
        lua_pop(state, 1);
        return 0;
    }
    if (!lua_istable(state, -1)) luaL_error(state, "%s must be a table", key);
    return 1;
}

static void read_filter(lua_State *state, GF_Filter *filter)
{
    if (!optional_table(state, "filter")) return;
    static const char *const modes[] = {"legacy", "lowpass", "bandpass", "highpass"};
    filter->mode = (GF_FilterMode)choice(state, "mode", modes, 4, GF_FILTER_LOW);
    filter->resonance = field(state, "resonance", filter->resonance, .5f, 12);
    filter->depth = field(state, "depth", 0, -8, 8);
    filter->velocity = field(state, "velocity", 0, -4, 4);
    filter->keytrack = field(state, "keytrack", 0, -2, 2);
    GF_ADSR *env = &filter->envelope;
    env->attack = field(state, "attack", env->attack, 0, 10);
    env->decay = field(state, "decay", env->decay, 0, 10);
    env->sustain = field(state, "sustain", env->sustain, 0, 1);
    env->release = field(state, "release", env->release, 0, 20);
    lua_pop(state, 1);
}

static void read_routes(lua_State *state, GF_Patch *patch)
{
    if (!optional_table(state, "mod")) return;
    static const char *const sources[GF_MOD_SOURCES] = {
        "lfo", "amp_env", "filter_env", "shape", "velocity", "key", "color", "morph", "echo"
    };
    static const char *const destinations[GF_MOD_DESTINATIONS] = {
        "cutoff", "pitch", "morph", "fm", "sample", "grain_position"
    };
    const size_t count = lua_rawlen(state, -1);
    if (count > GF_MOD_ROUTES) luaL_error(state, "mod accepts at most %d routes", GF_MOD_ROUTES);
    patch->route_count = (unsigned)count;
    for (unsigned i = 0; i < patch->route_count; ++i) {
        lua_rawgeti(state, -1, i + 1);
        if (!lua_istable(state, -1)) luaL_error(state, "each mod route must be a table");
        patch->routes[i] = (GF_ModRoute){
            .source = (GF_ModSource)choice(state, "source", sources, GF_MOD_SOURCES, -1),
            .destination = (GF_ModDestination)choice(state, "destination", destinations, GF_MOD_DESTINATIONS, -1),
            .amount = field(state, "amount", 0, -24, 24)
        };
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}

static void read_shape(lua_State *state, GF_Shape *shape)
{
    if (!optional_table(state, "shape")) return;
    const size_t count = lua_rawlen(state, -1);
    if (!count || count > GF_SEGMENTS) luaL_error(state, "shape needs 1 to %d segments", GF_SEGMENTS);
    shape->count = (unsigned)count;
    const int loop_type = lua_getfield(state, -1, "loop");
    if (loop_type != LUA_TNIL && loop_type != LUA_TBOOLEAN) luaL_error(state, "loop must be boolean");
    shape->loop = lua_toboolean(state, -1);
    lua_pop(state, 1);
    for (unsigned i = 0; i < shape->count; ++i) {
        lua_rawgeti(state, -1, i + 1);
        if (!lua_istable(state, -1)) luaL_error(state, "each shape segment must be a table");
        shape->segments[i] = (GF_Segment){
            .time = field(state, "time", .5f, .005f, 20),
            .level = field(state, "level", 0, -1, 1)
        };
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}

static void read_grain(lua_State *state, GF_Patch *patch)
{
    if (!optional_table(state, "grain")) return;
    if (!patch->sample_file[0]) luaL_error(state, "grain requires a sample_file");
    patch->grain = (GF_Granular){
        .size = field(state, "size", .12f, .015f, .24f),
        .density = field(state, "density", 24, 2, 100),
        .position = field(state, "position", patch->grain.position, 0, 1),
        .spray = field(state, "spray", .08f, 0, 1)
    };
    lua_pop(state, 1);
}

static int basename_only(const char *name)
{
    return strspn(name, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") == strlen(name);
}

static void read_patch(lua_State *state, GF_Patch *p)
{
    *p = gf_default_patch();
    string_field(state, "name", p->name, sizeof(p->name));
    string_field(state, "sample_file", p->sample_file, sizeof(p->sample_file));
    if (!basename_only(p->sample_file))
        luaL_error(state, "sample_file must be a filename in assets/sounds");
    p->wave_a = wave(state, "wave_a", p->wave_a);
    p->wave_b = wave(state, "wave_b", p->wave_b);
    p->attack = field(state, "attack", p->attack, 0, 10);
    p->decay = field(state, "decay", p->decay, 0, 10);
    p->sustain = field(state, "sustain", p->sustain, 0, 1);
    p->release = field(state, "release", p->release, 0, 20);
    p->morph = field(state, "morph", p->morph, 0, 1);
    p->color = field(state, "color", p->color, 0, 1);
    p->echo = field(state, "echo", p->echo, 0, 1);
    p->detune = field(state, "detune", p->detune, -1200, 1200);
    p->fm = field(state, "fm", p->fm, 0, 8);
    p->lfo = field(state, "lfo", p->lfo, 0, 20);
    p->motion = field(state, "motion", p->motion, 0, 1);
    p->sample = field(state, "sample", p->sample, 0, 1);
    read_filter(state, &p->filter);
    read_shape(state, &p->shape);
    read_routes(state, p);
    read_grain(state, p);
}

GF_Scene gf_default_scene(void)
{
    return (GF_Scene){
        .mesh = GF_DEFAULT_MESH, .tint = 0x8eb861, .accent = 0xcce77e,
        .spin = .48f, .tilt = .17f, .aspect = 1, .satellites = 3
    };
}

static uint32_t integer_field(lua_State *state, const char *key, uint32_t current, uint32_t max)
{
    const float number = field(state, key, (float)current, 0, (float)max);
    if (floorf(number) != number) luaL_error(state, "%s must be an integer", key);
    return (uint32_t)number;
}

static void read_scene(lua_State *state, GF_Scene *scene)
{
    *scene = gf_default_scene();
    if (lua_getfield(state, -1, "scene") != LUA_TNIL) {
        if (!lua_istable(state, -1)) luaL_error(state, "scene must be a table");
        string_field(state, "mesh", scene->mesh, sizeof(scene->mesh));
        const char *extension = strrchr(scene->mesh, '.');
        if (!basename_only(scene->mesh) || !extension || strcmp(extension, ".obj") != 0)
            luaL_error(state, "scene mesh must be an .obj filename in assets/graphics");
        scene->tint = integer_field(state, "tint", scene->tint, 0xffffff);
        scene->accent = integer_field(state, "accent", scene->accent, 0xffffff);
        scene->spin = field(state, "spin", scene->spin, -3, 3);
        scene->tilt = field(state, "tilt", scene->tilt, -.65f, .65f);
        scene->aspect = field(state, "aspect", scene->aspect, .75f, 1.25f);
        scene->satellites = integer_field(state, "satellites", scene->satellites, 5);
    }
    lua_pop(state, 1);
}

typedef struct {
    GF_Bank bank;
    const char *source;
    size_t length;
} BankSource;

static int execute_bank(lua_State *state)
{
    BankSource *input = lua_touserdata(state, 1);
    GF_Bank *bank = &input->bank;
    luaL_requiref(state, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(state, 1);
    if (luaL_loadbufferx(state, input->source, input->length, "@assets/scripts/bank.lua", "t") != LUA_OK)
        return lua_error(state);
    lua_sethook(state, limit, LUA_MASKCOUNT, 100000);
    lua_call(state, 0, 1);
    if (!lua_istable(state, -1)) return luaL_error(state, "bank must return a table");
    bank->count = lua_rawlen(state, -1);
    if (bank->count == 0 || bank->count > GF_PATCHES)
        return luaL_error(state, "bank must contain 1 to %d patches", GF_PATCHES);
    for (size_t i = 0; i < bank->count; ++i) {
        lua_rawgeti(state, -1, (lua_Integer)i + 1);
        if (!lua_istable(state, -1))
            return luaL_error(state, "patch %d must be a table", (int)i + 1);
        read_patch(state, &bank->patches[i]);
        read_scene(state, &bank->scenes[i]);
        lua_pop(state, 1);
    }
    return 0;
}

int gf_bank_parse(const char *source, size_t length, GF_Bank *bank, char *error, size_t size)
{
    if (!source) return report(error, size, "no Lua source");
    lua_State *state = luaL_newstate();
    if (!state) return report(error, size, "could not initialize Lua");
    BankSource *input = calloc(1, sizeof(*input));
    if (!input) {
        lua_close(state);
        return report(error, size, "could not allocate a patch bank");
    }
    input->source = source;
    input->length = length;
    lua_pushcfunction(state, execute_bank);
    lua_pushlightuserdata(state, input);
    const int ok = lua_pcall(state, 1, 0, 0) == LUA_OK;
    if (ok) {
        *bank = input->bank;
        if (error && size) error[0] = '\0';
    } else {
        report(error, size, lua_tostring(state, -1));
    }
    lua_close(state);
    free(input);
    return ok;
}
