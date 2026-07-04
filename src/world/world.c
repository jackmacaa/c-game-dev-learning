#include "world/world.h"
#include <stdlib.h>
#include <math.h>

typedef struct
{
    int x;
    int y;
    int width;
    int height;
    TileType tile_type;
} TileRect;

typedef struct
{
    int center_x;
    int center_y;
    int radius;
    TileType tile_type;
} TileCircle;

static void world_set_tile(World *world, int column, int row, TileType tile_type)
{
    if (!world)
    {
        return;
    }

    if (column < 0 || row < 0 || column >= world->columns || row >= world->rows)
    {
        return;
    }

    int tile_index = row * world->columns + column;
    world->tiles[tile_index] = (int)tile_type;
}

static void world_paint_rect(World *world, TileRect rect)
{
    for (int row = rect.y; row < rect.y + rect.height; row++)
    {
        for (int column = rect.x; column < rect.x + rect.width; column++)
        {
            world_set_tile(world, column, row, rect.tile_type);
        }
    }
}

static void world_paint_circle(World *world, TileCircle circle)
{
    for (int row = circle.center_y - circle.radius; row <= circle.center_y + circle.radius; row++)
    {
        for (int column = circle.center_x - circle.radius; column <= circle.center_x + circle.radius; column++)
        {
            int dx = column - circle.center_x;
            int dy = row - circle.center_y;
            if ((dx * dx) + (dy * dy) <= (circle.radius * circle.radius))
            {
                world_set_tile(world, column, row, circle.tile_type);
            }
        }
    }
}

static void world_paint_path_segment(World *world, int start_column, int start_row, int end_column, int end_row, int half_width)
{
    if (start_column == end_column)
    {
        int min_row = (start_row < end_row) ? start_row : end_row;
        int max_row = (start_row > end_row) ? start_row : end_row;

        for (int row = min_row; row <= max_row; row++)
        {
            for (int width = -half_width; width <= half_width; width++)
            {
                world_set_tile(world, start_column + width, row, TILE_PATH);
            }
        }
    }
    else if (start_row == end_row)
    {
        int min_column = (start_column < end_column) ? start_column : end_column;
        int max_column = (start_column > end_column) ? start_column : end_column;

        for (int column = min_column; column <= max_column; column++)
        {
            for (int width = -half_width; width <= half_width; width++)
            {
                world_set_tile(world, column, start_row + width, TILE_PATH);
            }
        }
    }
}

static void world_carve_mountain_spiral(World *world, int center_column, int center_row)
{
    // Spiral staircase from outer mountain ring to summit altar.
    world_paint_path_segment(world, center_column, center_row + 6, center_column - 6, center_row + 6, 0);
    world_paint_path_segment(world, center_column - 6, center_row + 6, center_column - 6, center_row - 5, 0);
    world_paint_path_segment(world, center_column - 6, center_row - 5, center_column + 5, center_row - 5, 0);
    world_paint_path_segment(world, center_column + 5, center_row - 5, center_column + 5, center_row - 2, 0);
    world_paint_path_segment(world, center_column + 5, center_row - 2, center_column - 3, center_row - 2, 0);
    world_paint_path_segment(world, center_column - 3, center_row - 2, center_column - 3, center_row + 1, 0);
    world_paint_path_segment(world, center_column - 3, center_row + 1, center_column + 2, center_row + 1, 0);
    world_paint_path_segment(world, center_column + 2, center_row + 1, center_column + 2, center_row, 0);
    world_paint_path_segment(world, center_column + 2, center_row, center_column, center_row, 0);
}

static void world_apply_hand_authored_layout(World *world)
{
    // Base layer: default plains that all authored regions override.
    for (int row = 0; row < world->rows; row++)
    {
        for (int column = 0; column < world->columns; column++)
        {
            world_set_tile(world, column, row, TILE_GRASS);
        }
    }

    // Region blocks are intentionally authored for easy iteration.
    const TileRect authored_regions[] = {
        {0, 0, world->columns, 10, TILE_SAND}, // northern dunes
        {35, 10, 15, 40, TILE_STONE},          // eastern highlands
        {0, 34, 13, 16, TILE_STONE},           // south-west cliffs
        {8, 16, 11, 10, TILE_SAND},            // central-west basin
        {18, 30, 10, 10, TILE_SAND},           // southern dry valley
        {2, 4, 32, 2, TILE_PATH},              // north highway
        {10, 10, 2, 26, TILE_PATH},            // west vertical road
        {12, 20, 24, 2, TILE_PATH},            // central horizontal road
        {24, 22, 2, 14, TILE_PATH},            // mountain approach road
        {24, 34, 16, 2, TILE_PATH},            // southern connector
        {6, 28, 16, 2, TILE_PATH},             // lower west connector
        {32, 6, 2, 16, TILE_PATH},             // east approach road
    };

    int region_count = (int)(sizeof(authored_regions) / sizeof(authored_regions[0]));
    for (int index = 0; index < region_count; index++)
    {
        world_paint_rect(world, authored_regions[index]);
    }

    int mountain_center_column = world->columns / 2;
    int mountain_center_row = world->rows / 2;

    // Mountain rings create a climb-like center objective area.
    const TileCircle authored_mountain[] = {
        {mountain_center_column, mountain_center_row, 6, TILE_STONE},
        {mountain_center_column, mountain_center_row, 4, TILE_MOUNTAIN},
        {mountain_center_column, mountain_center_row, 2, TILE_PATH},
    };

    int mountain_count = (int)(sizeof(authored_mountain) / sizeof(authored_mountain[0]));
    for (int index = 0; index < mountain_count; index++)
    {
        world_paint_circle(world, authored_mountain[index]);
    }

    world_carve_mountain_spiral(world, mountain_center_column, mountain_center_row);
}

static Color world_tile_color(TileType tile_type)
{
    switch (tile_type)
    {
    case TILE_SAND:
        return (Color){216, 197, 140, 255};
    case TILE_STONE:
        return (Color){157, 166, 173, 255};
    case TILE_PATH:
        return (Color){171, 128, 87, 255};
    case TILE_MOUNTAIN:
        return (Color){106, 99, 92, 255};
    case TILE_GRASS:
    default:
        return (Color){126, 193, 107, 255};
    }
}

static unsigned int world_hash(int x, int y)
{
    unsigned int seed = (unsigned int)(x * 374761393u + y * 668265263u);
    seed = (seed ^ (seed >> 13)) * 1274126177u;
    return seed ^ (seed >> 16);
}

static void world_draw_tile_texture(int column, int row, Rectangle tile_rect, TileType tile_type)
{
    unsigned int seed = world_hash(column, row);

    if (tile_type == TILE_GRASS)
    {
        for (int i = 0; i < 4; i++)
        {
            float offset_x = (float)((seed >> (i * 4)) & 15) / 15.0f;
            float offset_y = (float)((seed >> (i * 5 + 7)) & 15) / 15.0f;
            Vector2 p0 = {tile_rect.x + offset_x * tile_rect.width, tile_rect.y + offset_y * tile_rect.height};
            Vector2 p1 = {p0.x + 3.5f, p0.y - 7.0f};
            DrawLineEx(p0, p1, 1.3f, (Color){78, 150, 67, 140});
        }
    }
    else if (tile_type == TILE_SAND)
    {
        for (int i = 0; i < 3; i++)
        {
            float y = tile_rect.y + (float)(8 + i * 20);
            float wave_shift = (float)((seed >> (i * 3)) & 7);
            DrawLineBezier((Vector2){tile_rect.x + 8.0f, y}, (Vector2){tile_rect.x + tile_rect.width - 8.0f, y + wave_shift - 3.0f}, 1.2f, (Color){193, 170, 118, 120});
        }
    }
    else if (tile_type == TILE_STONE || tile_type == TILE_MOUNTAIN)
    {
        for (int i = 0; i < 3; i++)
        {
            float px = tile_rect.x + 10.0f + (float)(((seed >> (i * 6)) & 31) % 52);
            float py = tile_rect.y + 10.0f + (float)(((seed >> (i * 5 + 9)) & 31) % 52);
            float r = 2.0f + (float)((seed >> (i * 4 + 3)) & 3);
            DrawCircleV((Vector2){px, py}, r, (Color){84, 92, 101, 150});
        }
    }
    else if (tile_type == TILE_PATH)
    {
        DrawLineEx((Vector2){tile_rect.x + 7.0f, tile_rect.y + tile_rect.height * 0.35f},
                   (Vector2){tile_rect.x + tile_rect.width - 7.0f, tile_rect.y + tile_rect.height * 0.55f},
                   1.2f,
                   (Color){120, 90, 58, 130});
        DrawLineEx((Vector2){tile_rect.x + 7.0f, tile_rect.y + tile_rect.height * 0.62f},
                   (Vector2){tile_rect.x + tile_rect.width - 7.0f, tile_rect.y + tile_rect.height * 0.78f},
                   1.2f,
                   (Color){120, 90, 58, 100});
    }
}

TileType world_get_tile_type(const World *world, int tile_column, int tile_row)
{
    if (!world || tile_column < 0 || tile_row < 0 || tile_column >= world->columns || tile_row >= world->rows)
    {
        return TILE_GRASS;
    }

    int tile_index = tile_row * world->columns + tile_column;
    return (TileType)world->tiles[tile_index];
}

bool world_tile_is_blocking(TileType tile_type)
{
    return tile_type == TILE_STONE || tile_type == TILE_MOUNTAIN;
}

bool world_rect_collides_blocking_tiles(const World *world, Rectangle rect)
{
    if (!world)
    {
        return false;
    }

    float left = rect.x;
    float right = rect.x + rect.width;
    float top = rect.y;
    float bottom = rect.y + rect.height;

    int min_column = (int)floorf(left / (float)world->tile_size_px);
    int max_column = (int)floorf((right - 0.001f) / (float)world->tile_size_px);
    int min_row = (int)floorf(top / (float)world->tile_size_px);
    int max_row = (int)floorf((bottom - 0.001f) / (float)world->tile_size_px);

    for (int row = min_row; row <= max_row; row++)
    {
        for (int column = min_column; column <= max_column; column++)
        {
            TileType tile_type = world_get_tile_type(world, column, row);
            if (world_tile_is_blocking(tile_type))
            {
                return true;
            }
        }
    }

    return false;
}

bool world_circle_collides_blocking_tiles(const World *world, Vector2 center, float radius)
{
    if (!world || radius <= 0.0f)
    {
        return false;
    }

    float left = center.x - radius;
    float right = center.x + radius;
    float top = center.y - radius;
    float bottom = center.y + radius;

    int min_column = (int)floorf(left / (float)world->tile_size_px);
    int max_column = (int)floorf((right - 0.001f) / (float)world->tile_size_px);
    int min_row = (int)floorf(top / (float)world->tile_size_px);
    int max_row = (int)floorf((bottom - 0.001f) / (float)world->tile_size_px);

    for (int row = min_row; row <= max_row; row++)
    {
        for (int column = min_column; column <= max_column; column++)
        {
            TileType tile_type = world_get_tile_type(world, column, row);
            if (!world_tile_is_blocking(tile_type))
            {
                continue;
            }

            Rectangle tile_rect = {
                (float)(column * world->tile_size_px),
                (float)(row * world->tile_size_px),
                (float)world->tile_size_px,
                (float)world->tile_size_px};

            if (CheckCollisionCircleRec(center, radius, tile_rect))
            {
                return true;
            }
        }
    }

    return false;
}

World *world_create(int world_width_px, int world_height_px, int tile_size_px)
{
    if (world_width_px <= 0 || world_height_px <= 0 || tile_size_px <= 0)
    {
        return NULL;
    }

    World *world = (World *)malloc(sizeof(World));
    if (!world)
    {
        return NULL;
    }

    world->tile_size_px = tile_size_px;
    world->columns = world_width_px / tile_size_px;
    world->rows = world_height_px / tile_size_px;
    world->world_width_px = world->columns * tile_size_px;
    world->world_height_px = world->rows * tile_size_px;
    world->player_spawn = (Vector2){(float)(tile_size_px * 3), (float)(tile_size_px * 3)};
    world->mountain_center = (Vector2){(float)world->world_width_px * 0.5f, (float)world->world_height_px * 0.5f};

    int tile_count = world->columns * world->rows;
    world->tiles = (int *)malloc(sizeof(int) * tile_count);
    if (!world->tiles)
    {
        free(world);
        return NULL;
    }

    world_apply_hand_authored_layout(world);

    return world;
}

void world_render(const World *world)
{
    if (!world)
    {
        return;
    }

    for (int row = 0; row < world->rows; row++)
    {
        for (int column = 0; column < world->columns; column++)
        {
            Rectangle tile_rect = {
                (float)(column * world->tile_size_px),
                (float)(row * world->tile_size_px),
                (float)world->tile_size_px,
                (float)world->tile_size_px};

            TileType tile_type = world_get_tile_type(world, column, row);
            DrawRectangleRec(tile_rect, world_tile_color(tile_type));
            world_draw_tile_texture(column, row, tile_rect, tile_type);
            DrawRectangleLinesEx(tile_rect, 1.0f, (Color){76, 86, 95, 95});
        }
    }
}

void world_clamp_rect_to_bounds(const World *world, Rectangle *rect)
{
    if (!world || !rect)
    {
        return;
    }

    if (rect->x < 0.0f)
    {
        rect->x = 0.0f;
    }
    if (rect->y < 0.0f)
    {
        rect->y = 0.0f;
    }
    if (rect->x + rect->width > (float)world->world_width_px)
    {
        rect->x = (float)world->world_width_px - rect->width;
    }
    if (rect->y + rect->height > (float)world->world_height_px)
    {
        rect->y = (float)world->world_height_px - rect->height;
    }
}

void world_destroy(World *world)
{
    if (!world)
    {
        return;
    }

    free(world->tiles);
    world->tiles = NULL;
    free(world);
}
