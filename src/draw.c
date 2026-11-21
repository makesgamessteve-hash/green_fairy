#include "draw.h"

#include <SDL3/SDL.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#define gl(name) gl##name
#else
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#define gl(name) pgl##name
#define GL_FUNCTIONS(X) \
    X(void, Viewport, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void, Scissor, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void, Enable, (GLenum cap)) X(void, Disable, (GLenum cap)) \
    X(void, ClearColor, (GLfloat r, GLfloat g, GLfloat b, GLfloat a)) \
    X(void, Clear, (GLbitfield mask)) X(void, BlendFunc, (GLenum a, GLenum b)) \
    X(void, GenTextures, (GLsizei n, GLuint *textures)) \
    X(void, DeleteTextures, (GLsizei n, const GLuint *textures)) \
    X(void, ActiveTexture, (GLenum unit)) \
    X(void, BindTexture, (GLenum target, GLuint texture)) \
    X(void, TexParameteri, (GLenum target, GLenum pname, GLint value)) \
    X(void, TexImage2D, (GLenum target, GLint level, GLint format, GLsizei w, GLsizei h, \
                         GLint border, GLenum input, GLenum type, const void *pixels)) \
    X(GLuint, CreateShader, (GLenum type)) \
    X(void, ShaderSource, (GLuint shader, GLsizei count, const GLchar *const *source, const GLint *length)) \
    X(void, CompileShader, (GLuint shader)) X(void, DeleteShader, (GLuint shader)) \
    X(void, GetShaderiv, (GLuint shader, GLenum name, GLint *value)) \
    X(void, GetShaderInfoLog, (GLuint shader, GLsizei max, GLsizei *len, GLchar *info)) \
    X(GLuint, CreateProgram, (void)) X(void, DeleteProgram, (GLuint program)) \
    X(void, AttachShader, (GLuint program, GLuint shader)) X(void, LinkProgram, (GLuint program)) \
    X(void, GetProgramiv, (GLuint program, GLenum name, GLint *value)) \
    X(void, GetProgramInfoLog, (GLuint program, GLsizei max, GLsizei *len, GLchar *info)) \
    X(void, UseProgram, (GLuint program)) \
    X(GLint, GetUniformLocation, (GLuint program, const GLchar *name)) \
    X(void, Uniform1i, (GLint location, GLint value)) \
    X(void, UniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)) \
    X(void, GenBuffers, (GLsizei n, GLuint *buffers)) \
    X(void, DeleteBuffers, (GLsizei n, const GLuint *buffers)) \
    X(void, BindBuffer, (GLenum target, GLuint buffer)) \
    X(void, BufferData, (GLenum target, GLsizeiptr size, const void *data, GLenum usage)) \
    X(void, GenVertexArrays, (GLsizei n, GLuint *arrays)) \
    X(void, DeleteVertexArrays, (GLsizei n, const GLuint *arrays)) \
    X(void, BindVertexArray, (GLuint array)) \
    X(void, EnableVertexAttribArray, (GLuint index)) \
    X(void, VertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, \
                                  GLsizei stride, const void *offset)) \
    X(void, DrawArrays, (GLenum mode, GLint first, GLsizei count))
#define DECLARE(result, name, args) static result (APIENTRYP pgl##name) args;
GL_FUNCTIONS(DECLARE)
#undef DECLARE
#endif

enum { VERTEX_LIMIT = 40000, ATLAS_W = 192, ATLAS_H = 160 };
typedef struct { float x, y, u, v; Color color; } Vertex;
static Vertex vertices[VERTEX_LIMIT];
static size_t count;
static GLuint program, vao, vbo, atlas;
static GLint projection_location;

Color ink(unsigned rgb)
{
    return (Color){((rgb >> 16) & 255) / 255.f, ((rgb >> 8) & 255) / 255.f,
                   (rgb & 255) / 255.f, 1};
}

Color alpha(Color color, float opacity)
{
    color.a *= opacity;
    return color;
}

Color blend(Color a, Color b, float ratio)
{
    return (Color){a.r + (b.r-a.r)*ratio, a.g + (b.g-a.g)*ratio,
                   a.b + (b.b-a.b)*ratio, a.a + (b.a-a.a)*ratio};
}

bool inside(Rect r, float x, float y)
{
    return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
}

static GLuint compile(GLenum type, const char *source)
{
    const GLuint shader = gl(CreateShader)(type);
    gl(ShaderSource)(shader, 1, &source, NULL);
    gl(CompileShader)(shader);
    GLint ok;
    gl(GetShaderiv)(shader, GL_COMPILE_STATUS, &ok);
    if (ok) return shader;
    char message[1024];
    gl(GetShaderInfoLog)(shader, sizeof(message), NULL, message);
    SDL_Log("Shader: %s", message);
    gl(DeleteShader)(shader);
    return 0;
}

bool draw_init(const char *atlas_path)
{
#ifndef __EMSCRIPTEN__
#define LOAD(result, name, args) \
    pgl##name = (result (APIENTRYP) args)SDL_GL_GetProcAddress("gl" #name); \
    if (!pgl##name) { SDL_Log("OpenGL function missing: gl%s", #name); return false; }
    GL_FUNCTIONS(LOAD)
#undef LOAD
#endif

#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 300 es\nprecision mediump float;\n"
#else
#define SHADER_VERSION "#version 330 core\n"
#endif
    const char *vertex_source = SHADER_VERSION
        "layout(location=0) in vec2 point; layout(location=1) in vec2 uv;"
        "layout(location=2) in vec4 tint; uniform mat4 projection;"
        "out vec2 st; out vec4 color;"
        "void main(){ st=uv; color=tint; gl_Position=projection*vec4(point,0.,1.); }";
    const char *fragment_source = SHADER_VERSION
        "in vec2 st; in vec4 color; uniform sampler2D atlas; out vec4 fragment;"
        "void main(){ float mask=st.x < 0. ? 1. : texture(atlas,st).r;"
        "fragment=vec4(color.rgb,color.a*mask); }";
    const GLuint vertex = compile(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment = compile(GL_FRAGMENT_SHADER, fragment_source);
    if (vertex && fragment) {
        program = gl(CreateProgram)();
        gl(AttachShader)(program, vertex);
        gl(AttachShader)(program, fragment);
        gl(LinkProgram)(program);
    }
    if (vertex) gl(DeleteShader)(vertex);
    if (fragment) gl(DeleteShader)(fragment);
    if (!program) return false;
    GLint linked;
    gl(GetProgramiv)(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char message[1024];
        gl(GetProgramInfoLog)(program, sizeof(message), NULL, message);
        SDL_Log("OpenGL program: %s", message);
        return false;
    }

    SDL_Surface *bitmap = SDL_LoadBMP(atlas_path);
    if (!bitmap) return false;
    SDL_Surface *rgba = SDL_ConvertSurface(bitmap, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(bitmap);
    if (!rgba) return false;
    if (rgba->w != ATLAS_W || rgba->h != ATLAS_H) {
        SDL_DestroySurface(rgba);
        SDL_SetError("studio.bmp must be %d x %d", ATLAS_W, ATLAS_H);
        return false;
    }
    unsigned char pixels[ATLAS_W * ATLAS_H];
    for (int y = 0; y < ATLAS_H; ++y) {
        const unsigned char *row = (const unsigned char *)rgba->pixels + y * rgba->pitch;
        for (int x = 0; x < ATLAS_W; ++x) pixels[y * ATLAS_W + x] = row[x * 4];
    }
    SDL_DestroySurface(rgba);
    gl(GenTextures)(1, &atlas);
    gl(BindTexture)(GL_TEXTURE_2D, atlas);
    gl(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl(TexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl(TexImage2D)(GL_TEXTURE_2D, 0, GL_R8, ATLAS_W, ATLAS_H, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

    gl(GenVertexArrays)(1, &vao);
    gl(BindVertexArray)(vao);
    gl(GenBuffers)(1, &vbo);
    gl(BindBuffer)(GL_ARRAY_BUFFER, vbo);
    gl(EnableVertexAttribArray)(0);
    gl(VertexAttribPointer)(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, x));
    gl(EnableVertexAttribArray)(1);
    gl(VertexAttribPointer)(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, u));
    gl(EnableVertexAttribArray)(2);
    gl(VertexAttribPointer)(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
    gl(UseProgram)(program);
    projection_location = gl(GetUniformLocation)(program, "projection");
    gl(Uniform1i)(gl(GetUniformLocation)(program, "atlas"), 0);
    return true;
}

void draw_forget_context(void)
{
    vbo = vao = atlas = program = 0;
}

void draw_destroy(void)
{
    if (vbo) gl(DeleteBuffers)(1, &vbo);
    if (vao) gl(DeleteVertexArrays)(1, &vao);
    if (atlas) gl(DeleteTextures)(1, &atlas);
    if (program) gl(DeleteProgram)(program);
    draw_forget_context();
}

void draw_begin(void) { count = 0; }

void draw_end(const float projection[16], int x, int y, int width, int height, Color bg)
{
    gl(Viewport)(x, y, width, height);
    gl(Enable)(GL_SCISSOR_TEST);
    gl(Scissor)(x, y, width, height);
    gl(ClearColor)(bg.r, bg.g, bg.b, bg.a);
    gl(Clear)(GL_COLOR_BUFFER_BIT);
    gl(Disable)(GL_SCISSOR_TEST);
    gl(Enable)(GL_BLEND);
    gl(BlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl(UseProgram)(program);
    gl(ActiveTexture)(GL_TEXTURE0);
    gl(BindTexture)(GL_TEXTURE_2D, atlas);
    gl(BindVertexArray)(vao);
    gl(BindBuffer)(GL_ARRAY_BUFFER, vbo);
    gl(BufferData)(GL_ARRAY_BUFFER, (GLsizeiptr)(count * sizeof(*vertices)), vertices, GL_STREAM_DRAW);
    gl(UniformMatrix4fv)(projection_location, 1, GL_FALSE, projection);
    gl(DrawArrays)(GL_TRIANGLES, 0, (GLsizei)count);
}

static void triangle(Vertex a, Vertex b, Vertex c)
{
    if (count + 3 > VERTEX_LIMIT) return;
    vertices[count++] = a;
    vertices[count++] = b;
    vertices[count++] = c;
}

static void quad(Vertex a, Vertex b, Vertex c, Vertex d)
{
    triangle(a, b, c);
    triangle(a, c, d);
}

static Vertex solid(float x, float y, Color color)
{
    return (Vertex){x, y, -1, 0, color};
}

void shade(Rect r, Color top, Color bottom)
{
    quad(solid(r.x, r.y, top), solid(r.x + r.w, r.y, top),
         solid(r.x + r.w, r.y + r.h, bottom), solid(r.x, r.y + r.h, bottom));
}

void box(Rect r, Color color) { shade(r, color, color); }

void frame(Rect r, Color c)
{
    box((Rect){r.x, r.y, r.w, 1}, c);
    box((Rect){r.x, r.y + r.h - 1, r.w, 1}, c);
    box((Rect){r.x, r.y + 1, 1, r.h - 2}, c);
    box((Rect){r.x + r.w - 1, r.y + 1, 1, r.h - 2}, c);
}

void line(float x1, float y1, float x2, float y2, float width, Color c)
{
    const float length = hypotf(x2 - x1, y2 - y1);
    if (length < .01f) return;
    const float dx = (y2 - y1) * width / (2 * length);
    const float dy = (x1 - x2) * width / (2 * length);
    quad(solid(x1 + dx, y1 + dy, c), solid(x1 - dx, y1 - dy, c),
         solid(x2 - dx, y2 - dy, c), solid(x2 + dx, y2 + dy, c));
}

static void tile(Rect r, float x, float y, float w, float h, Color c)
{
    const float u = x / ATLAS_W, v = y / ATLAS_H;
    const float u2 = (x + w) / ATLAS_W, v2 = (y + h) / ATLAS_H;
    quad((Vertex){r.x, r.y, u, v, c}, (Vertex){r.x + r.w, r.y, u2, v, c},
         (Vertex){r.x + r.w, r.y + r.h, u2, v2, c}, (Vertex){r.x, r.y + r.h, u, v2, c});
}

void text(float x, float y, float size, Color c, const char *message)
{
    const float w = size * .6f;
    for (const unsigned char *ch = (const unsigned char *)message; *ch; ++ch) {
        if ((*ch & 0xc0) == 0x80) continue;
        const int glyph = (*ch >= 32 && *ch <= 126 ? *ch : '?') - 32;
        if (glyph) tile((Rect){x, y, w, size}, (float)(glyph % 16 * 12),
                        (float)(glyph / 16 * 20), 12, 20, c);
        x += w;
    }
}

float text_width(float size, const char *message)
{
    size_t length = 0;
    for (const unsigned char *ch = (const unsigned char *)message; *ch; ++ch)
        length += (*ch & 0xc0) != 0x80;
    return length * size * .6f;
}

void sprite(Rect rect, int index, Color color)
{
    if (index == 2) tile(rect, 85, 138, 8, 8, color);
    else tile(rect, (float)(index * 40), 124, 33, 33, color);
}

bool mesh_load(Mesh *mesh, const char *path)
{
    size_t length;
    char *source = SDL_LoadFile(path, &length);
    if (!source) return false;
    memset(mesh, 0, sizeof(*mesh));
    bool valid = length <= 256 * 1024 && !memchr(source, 0, length);
    char *row = source;
    while (valid && *row) {
        char *next = strchr(row, '\n');
        if (next) *next++ = '\0';
        if (strlen(row) > 255) {
            valid = false;
            break;
        }
        if (row[0] == 'v' && row[1] == ' ') {
            Point3 p;
            valid = mesh->point_count < GF_MESH_POINTS &&
                sscanf(row + 2, "%f%f%f", &p.x, &p.y, &p.z) == 3 &&
                isfinite(p.x) && isfinite(p.y) && isfinite(p.z) &&
                fabsf(p.x) <= 2 && fabsf(p.y) <= 2 && fabsf(p.z) <= 2;
            if (valid) mesh->points[mesh->point_count++] = p;
        } else if (row[0] == 'f' && row[1] == ' ') {
            unsigned a, b, c;
            char extra;
            valid = mesh->face_count < GF_MESH_FACES &&
                sscanf(row + 2, "%u%u%u %c", &a, &b, &c, &extra) == 3 &&
                a > 0 && a <= mesh->point_count && b > 0 && b <= mesh->point_count &&
                c > 0 && c <= mesh->point_count;
            if (valid) {
                unsigned *face = mesh->faces[mesh->face_count++];
                face[0] = a - 1;
                face[1] = b - 1;
                face[2] = c - 1;
            }
        }
        if (!next) break;
        row = next;
    }
    SDL_free(source);
    return valid && mesh->face_count > 0;
}

typedef struct { size_t index; float depth; Color color; } Surface;

static int depth_order(const void *a, const void *b)
{
    const float delta = ((const Surface *)a)->depth - ((const Surface *)b)->depth;
    return (delta > 0) - (delta < 0);
}

void mesh_draw(const Mesh *mesh, float x, float y, float scale, float angle, Color tint)
{
    Point3 world[GF_MESH_POINTS], projected[GF_MESH_POINTS];
    Surface surfaces[GF_MESH_FACES];
    const float sine = sinf(angle), cosine = cosf(angle);
    for (size_t i = 0; i < mesh->point_count; ++i) {
        const Point3 p = mesh->points[i];
        const float a = cosine * p.x + sine * p.z;
        const float depth = cosine * p.z - sine * p.x;
        const Point3 rotated = {a * .985f - p.y * .17f, a * .17f + p.y * .985f, depth};
        const float perspective = scale * (5 / (5 - rotated.z));
        world[i] = rotated;
        projected[i] = (Point3){x + rotated.x * perspective, y - rotated.y * perspective, depth};
    }
    for (size_t i = 0; i < mesh->face_count; ++i) {
        const unsigned *indices = mesh->faces[i];
        const Point3 a = world[indices[0]], b = world[indices[1]], c = world[indices[2]];
        const Point3 u = {b.x-a.x, b.y-a.y, b.z-a.z}, v = {c.x-a.x, c.y-a.y, c.z-a.z};
        const Point3 n = {u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x};
        const float length = sqrtf(n.x*n.x + n.y*n.y + n.z*n.z);
        const float lit = length > .001f ? fabsf((-.29f*n.x + .64f*n.y + .58f*n.z) / length) : 0;
        const Color base = blend(ink(0x142820), tint, .16f + .81f * lit);
        surfaces[i] = (Surface){i, (a.z+b.z+c.z)/3, base};
    }
    qsort(surfaces, mesh->face_count, sizeof(*surfaces), depth_order);
    for (size_t i = 0; i < mesh->face_count; ++i) {
        const unsigned *face = mesh->faces[surfaces[i].index];
        const Point3 a = projected[face[0]], b = projected[face[1]], c = projected[face[2]];
        const Color edge = alpha(blend(surfaces[i].color, tint, .8f), .45f);
        triangle(solid(a.x, a.y, surfaces[i].color), solid(b.x, b.y, surfaces[i].color),
                 solid(c.x, c.y, surfaces[i].color));
        line(a.x, a.y, b.x, b.y, .8f, edge);
        line(b.x, b.y, c.x, c.y, .8f, edge);
        line(c.x, c.y, a.x, a.y, .8f, edge);
    }
}
