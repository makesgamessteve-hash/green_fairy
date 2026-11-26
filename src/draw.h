#ifndef GF_DRAW_H
#define GF_DRAW_H

#include <stdbool.h>
#include <stddef.h>

typedef struct { float x, y, w, h; } Rect;
typedef struct { float x, y, z; } Point3;
typedef struct { float r, g, b, a; } Color;
typedef struct { float yaw, tilt, aspect; } MeshPose;

enum { GF_MESH_POINTS = 256, GF_MESH_FACES = 512 };
typedef struct {
    Point3 points[GF_MESH_POINTS];
    unsigned faces[GF_MESH_FACES][3];
    size_t point_count, face_count;
} Mesh;

Color ink(unsigned rgb);
Color alpha(Color color, float opacity);
Color blend(Color a, Color b, float ratio);
bool inside(Rect r, float x, float y);
bool draw_init(const char *atlas_path);
void draw_destroy(void);
void draw_forget_context(void);
void draw_begin(void);
void draw_end(const float projection[16], int x, int y, int width, int height, Color background);
void box(Rect rect, Color color);
void shade(Rect rect, Color top, Color bottom);
void frame(Rect rect, Color color);
void line(float x1, float y1, float x2, float y2, float thickness, Color color);
void text(float x, float y, float size, Color color, const char *message);
float text_width(float size, const char *message);
void sprite(Rect rect, int index, Color color);
void star(float x, float y, float radius, Color color, bool filled);
bool mesh_load(Mesh *mesh, const char *path);
void mesh_draw(const Mesh *mesh, float x, float y, float scale, MeshPose pose, Color tint, Color accent);

#endif
