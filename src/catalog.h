#ifndef GF_CATALOG_H
#define GF_CATALOG_H

#include "synth.h"

#include <stdbool.h>

typedef struct {
    uint32_t ids[GF_PATCHES];
    size_t count;
} GF_Favorites;

typedef struct {
    size_t indices[GF_PATCHES];
    bool starred[GF_PATCHES];
    size_t count, position;
    int type_count;
} GF_Catalog;

extern const char *const gf_type_names[GF_TYPE_COUNT];

bool gf_favorite_set(GF_Favorites *favorites, uint32_t id, bool starred);
void gf_catalog_update(GF_Catalog *catalog, const GF_Bank *bank, const GF_Favorites *favorites,
                       GF_PresetType type, size_t selected);

#endif
