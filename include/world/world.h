#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include <raylib.h>

typedef enum
{
    TILE_GRASS = 0,
    TILE_SAND,
    TILE_STONE,
    TILE_PATH,
    TILE_MOUNTAIN
} TileType;

typedef struct
{
    int columns;
    int rows;
    int tile_size_px;
    int world_width_px;
    int world_height_px;
    int *tiles;
    Vector2 player_spawn;
    Vector2 mountain_center;
} World;

// Allocate and build a hand-authored open world tile layout.
World *world_create(int world_width_px, int world_height_px, int tile_size_px);

// Render the complete world map in world coordinates.
void world_render(const World *world);

// Convert world-space coordinates to tile index values.
TileType world_get_tile_type(const World *world, int tile_column, int tile_row);

// Returns true if this tile type blocks movement.
bool world_tile_is_blocking(TileType tile_type);

// Returns true when any blocking tile overlaps the world-space rectangle.
bool world_rect_collides_blocking_tiles(const World *world, Rectangle rect);

// Returns true when any blocking tile overlaps the world-space circle.
bool world_circle_collides_blocking_tiles(const World *world, Vector2 center, float radius);

// Clamp an axis-aligned rectangle to world bounds.
void world_clamp_rect_to_bounds(const World *world, Rectangle *rect);

// Release world-owned heap memory.
void world_destroy(World *world);

#endif // WORLD_H
