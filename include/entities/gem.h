#ifndef GEM_H
#define GEM_H

#include <stdbool.h>
#include <raylib.h>

typedef enum
{
    GEM_RED = 0,
    GEM_BLUE,
    GEM_YELLOW,
    GEM_ORANGE,
    GEM_GREEN,
    GEM_PURPLE,
    GEM_COUNT
} GemType;

typedef struct
{
    GemType type;
    Vector2 position;
    float radius;
    bool collected;
} Gem;

// Initialize one world-space collectible gem.
void gem_initialize(Gem *gem, GemType type, float x, float y, float radius);

// Attempt to collect gem using player world-space collision circle.
bool gem_try_collect(Gem *gem, Vector2 player_collision_center, float player_collision_radius);

// Render gem as a colored triangle in world space.
void gem_render(const Gem *gem);

// Name used by HUD text for this gem color.
const char *gem_type_name(GemType type);

#endif // GEM_H
