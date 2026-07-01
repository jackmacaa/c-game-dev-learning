#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <raylib.h>
#include "entities/entity.h"

typedef struct
{
    Entity body;
    float move_speed_units_per_sec;
    Vector2 facing;
    int sprite_col;
    int sprite_row;

    Texture2D texture;
    bool texture_loaded;
    int sprite_columns;
    int sprite_rows;
    float frame_duration;
    float frame_timer;
} Player;

// Initialize gameplay and rendering state for the player.
void player_initialize(Player *player, float start_position_x, float start_position_y, float render_width, float render_height, const char *sprite_texture_path, int sprite_columns, int sprite_rows);

// Read keyboard state and convert it into player velocity and facing direction.
void player_apply_movement_input(Player *player);

// Advance movement and animation timers for one frame.
void player_update_frame(Player *player, float delta_time_seconds);

// Draw the player sprite (or fallback shape if texture is missing).
void player_render(const Player *player);

// Release GPU resources owned by the player.
void player_unload(Player *player);

#endif // PLAYER_H
