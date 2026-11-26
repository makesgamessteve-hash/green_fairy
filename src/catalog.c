#include "catalog.h"

const char *const gf_type_names[GF_TYPE_COUNT] = {
    "All presets", "Favorites", "Pads", "Leads", "Strings", "Choir", "Bass",
    "Keys & Bells", "Plucks", "Rhythms", "Percussion", "Drones & FX", "Other"
};

static size_t favorite_index(const GF_Favorites *favorites, uint32_t id)
{
    for (size_t i = 0; i < favorites->count; ++i)
        if (favorites->ids[i] == id) return i;
    return favorites->count;
}

bool gf_favorite_set(GF_Favorites *favorites, uint32_t id, bool starred)
{
    if (!id) return false;
    const size_t i = favorite_index(favorites, id);
    if (!starred) {
        if (i < favorites->count) favorites->ids[i] = favorites->ids[--favorites->count];
        return true;
    }
    if (i < favorites->count) return true;
    if (favorites->count == GF_PATCHES) return false;
    favorites->ids[favorites->count++] = id;
    return true;
}

void gf_catalog_update(GF_Catalog *catalog, const GF_Bank *bank, const GF_Favorites *favorites,
                       GF_PresetType type, size_t selected)
{
    catalog->count = 0;
    catalog->position = SIZE_MAX;
    catalog->type_count = GF_TYPE_OTHER;
    for (size_t i = 0; i < bank->count; ++i) {
        const GF_Patch *patch = &bank->patches[i];
        if (patch->type == GF_TYPE_OTHER) catalog->type_count = GF_TYPE_COUNT;
        const bool starred = favorite_index(favorites, patch->id) < favorites->count;
        catalog->starred[i] = starred;
        if (type != GF_TYPE_ALL && type != patch->type && !(type == GF_TYPE_FAVORITES && starred))
            continue;
        if (i == selected) catalog->position = catalog->count;
        catalog->indices[catalog->count++] = i;
    }
}
