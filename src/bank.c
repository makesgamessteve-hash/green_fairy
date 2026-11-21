#include "synth.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <math.h>
#include <stdio.h>
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

static GF_Wave wave(lua_State *state, const char *key, GF_Wave current)
{
    static const char *const names[] = {"sine", "triangle", "saw", "square", "noise"};
    char name[GF_NAME] = {0};
    string_field(state, key, name, sizeof(name));
    if (!name[0]) return current;
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        if (strcmp(name, names[i]) == 0) return (GF_Wave)i;
    return luaL_error(state, "unrecognized waveform for %s", key);
}

static void read_patch(lua_State *state, GF_Patch *p)
{
    *p = gf_default_patch();
    string_field(state, "name", p->name, sizeof(p->name));
    string_field(state, "sample_file", p->sample_file, sizeof(p->sample_file));
    if (p->sample_file[0] &&
        strspn(p->sample_file, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.") !=
        strlen(p->sample_file))
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
        lua_pop(state, 1);
    }
    return 0;
}

int gf_bank_parse(const char *source, size_t length, GF_Bank *bank, char *error, size_t size)
{
    if (!source) return report(error, size, "no Lua source");
    lua_State *state = luaL_newstate();
    if (!state) return report(error, size, "could not initialize Lua");
    BankSource input = {.source = source, .length = length};
    lua_pushcfunction(state, execute_bank);
    lua_pushlightuserdata(state, &input);
    const int ok = lua_pcall(state, 1, 0, 0) == LUA_OK;
    if (ok) {
        *bank = input.bank;
        if (error && size) error[0] = '\0';
    } else {
        report(error, size, lua_tostring(state, -1));
    }
    lua_close(state);
    return ok;
}
