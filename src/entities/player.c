#include "entities/player.h"
#include "core/config.h"
#include <math.h>

void player_initialize(Player *player, float start_position_x, float start_position_y, float render_width, float render_height, const char *sprite_texture_path, int sprite_columns, int sprite_rows)
{
    // Set base transform/collision body used by gameplay and rendering.
    entity_initialize(&player->body, start_position_x, start_position_y, render_width, render_height);

    // Player movement tuning and initial facing direction.
    player->move_speed_units_per_sec = 500.0f;
    player->facing = (Vector2){1.0f, 0.0f};
    player->is_sprinting = false;
    player->max_stamina = 100.0f;
    player->stamina = player->max_stamina;
    player->stamina_regen_cooldown_seconds = 0.0f;
    player->max_health = 5;
    player->health = player->max_health;
    player->damage_invulnerability_timer = 0.0f;
    player->damage_flash_timer = 0.0f;
    player->sprite_col = PLAYER_DOWN_COL;
    player->sprite_row = PLAYER_DOWN_ROW;

    // Sprite-sheet and texture runtime state.
    player->texture = (Texture2D){0};
    player->texture_loaded = false;

    player->sprite_columns = (sprite_columns > 0) ? sprite_columns : 1;
    player->sprite_rows = (sprite_rows > 0) ? sprite_rows : 1;
    player->frame_duration = 0.12f;
    player->frame_timer = 0.0f;

    if (sprite_texture_path && FileExists(sprite_texture_path))
    {
        player->texture = LoadTexture(sprite_texture_path);
        player->texture_loaded = (player->texture.id != 0);
    }
}

void player_apply_movement_input(Player *player)
{
    // Remember previous row so direction changes can restart animation cleanly.
    int previous_row = player->sprite_row;

    // Rebuild velocity from current keyboard state every frame.
    float input_x = 0.0f;
    float input_y = 0.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        input_x += 1.0f;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        input_x -= 1.0f;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        input_y += 1.0f;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        input_y -= 1.0f;
    }

    // Normalize input so diagonal movement has the same total speed.
    float input_length = sqrtf(input_x * input_x + input_y * input_y);
    if (input_length > 0.0f)
    {
        input_x /= input_length;
        input_y /= input_length;
    }

    bool wants_sprint = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    player->is_sprinting = wants_sprint && player->stamina > 0.0f;
    float current_speed = player->move_speed_units_per_sec * (player->is_sprinting ? 1.45f : 1.0f);

    player->body.velocity_x = input_x * current_speed;
    player->body.velocity_y = input_y * current_speed;

    // Choose the active direction row from movement intent.
    if (player->body.velocity_x > 0.0f)
    {
        player->facing = (Vector2){1.0f, 0.0f};
        player->sprite_row = PLAYER_RIGHT_ROW;
    }
    else if (player->body.velocity_x < 0.0f)
    {
        player->facing = (Vector2){-1.0f, 0.0f};
        player->sprite_row = PLAYER_LEFT_ROW;
    }
    else if (player->body.velocity_y > 0.0f)
    {
        player->sprite_row = PLAYER_DOWN_ROW;
    }
    else if (player->body.velocity_y < 0.0f)
    {
        player->sprite_row = PLAYER_UP_ROW;
    }

    // Restart row animation when direction changes.
    if (player->sprite_row != previous_row)
    {
        player->sprite_col = 0;
        player->frame_timer = 0.0f;
    }
}

void player_update_frame(Player *player, float delta_time_seconds)
{
    if (player->damage_invulnerability_timer > 0.0f)
    {
        player->damage_invulnerability_timer -= delta_time_seconds;
        if (player->damage_invulnerability_timer < 0.0f)
        {
            player->damage_invulnerability_timer = 0.0f;
        }
    }

    if (player->damage_flash_timer > 0.0f)
    {
        player->damage_flash_timer -= delta_time_seconds;
        if (player->damage_flash_timer < 0.0f)
        {
            player->damage_flash_timer = 0.0f;
        }
    }

    bool moving = fabsf(player->body.velocity_x) > 0.1f || fabsf(player->body.velocity_y) > 0.1f;
    if (player->is_sprinting && moving)
    {
        player->stamina -= 36.0f * delta_time_seconds;
        if (player->stamina < 0.0f)
        {
            player->stamina = 0.0f;
            player->is_sprinting = false;
        }
        player->stamina_regen_cooldown_seconds = 0.8f;
    }
    else
    {
        if (player->stamina_regen_cooldown_seconds > 0.0f)
        {
            player->stamina_regen_cooldown_seconds -= delta_time_seconds;
            if (player->stamina_regen_cooldown_seconds < 0.0f)
            {
                player->stamina_regen_cooldown_seconds = 0.0f;
            }
        }
        else
        {
            player->stamina += 22.0f * delta_time_seconds;
            if (player->stamina > player->max_stamina)
            {
                player->stamina = player->max_stamina;
            }
        }
    }

    // Move with velocity integration and keep player inside screen bounds.
    entity_update_kinematics(&player->body, delta_time_seconds);

    // Advance walk cycle columns while moving.
    if (player->sprite_columns > 1)
    {
        float speed_sq = player->body.velocity_x * player->body.velocity_x + player->body.velocity_y * player->body.velocity_y;
        bool is_moving = speed_sq > 1.0f;

        if (is_moving)
        {
            player->frame_timer += delta_time_seconds;
            if (player->frame_timer >= player->frame_duration)
            {
                player->frame_timer = 0.0f;
                player->sprite_col = (player->sprite_col + 1) % player->sprite_columns;
            }
        }
        else
        {
            if (player->sprite_row == PLAYER_DOWN_ROW)
                player->sprite_col = PLAYER_DOWN_COL;
            else if (player->sprite_row == PLAYER_LEFT_ROW)
                player->sprite_col = PLAYER_LEFT_COL;
            else if (player->sprite_row == PLAYER_RIGHT_ROW)
                player->sprite_col = PLAYER_RIGHT_COL;
            else
                player->sprite_col = PLAYER_UP_COL;

            player->frame_timer = 0.0f;
        }
    }
}

bool player_apply_damage(Player *player, int damage_amount, float invulnerability_seconds)
{
    if (!player || damage_amount <= 0)
    {
        return false;
    }

    if (player->damage_invulnerability_timer > 0.0f || player->health <= 0)
    {
        return false;
    }

    player->health -= damage_amount;
    if (player->health < 0)
    {
        player->health = 0;
    }

    player->damage_invulnerability_timer = invulnerability_seconds;
    player->damage_flash_timer = 0.16f;
    return true;
}

bool player_is_dead(const Player *player)
{
    return !player || player->health <= 0;
}

void player_render(const Player *player)
{
    // Preferred path: render sprite-sheet frame.
    if (player->texture_loaded)
    {
        float frame_width = (float)player->texture.width / (float)player->sprite_columns;
        float frame_height = (float)player->texture.height / (float)player->sprite_rows;
        int row_index = player->sprite_row;
        if (row_index < 0)
        {
            row_index = 0;
        }
        if (row_index >= player->sprite_rows)
        {
            row_index = player->sprite_rows - 1;
        }

        Rectangle source = {
            frame_width * player->sprite_col,
            frame_height * row_index,
            frame_width,
            frame_height};

        Rectangle destination = {
            player->body.position_x,
            player->body.position_y,
            player->body.render_width,
            player->body.render_height};

        Color tint = (player->damage_flash_timer > 0.0f) ? (Color){255, 145, 145, 255} : WHITE;
        DrawTexturePro(player->texture, source, destination, (Vector2){0.0f, 0.0f}, 0.0f, tint);
        return;
    }

    // Fallback path when texture file is missing or failed to load.
    DrawCircle((int)player->body.position_x, (int)player->body.position_y, player->body.render_width / 2.0f, BLUE);
}

void player_unload(Player *player)
{
    // Release GPU texture resource if it was loaded.
    if (player->texture_loaded)
    {
        UnloadTexture(player->texture);
        player->texture_loaded = false;
    }
}
