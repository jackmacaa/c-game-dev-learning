#include "core/game.h"
#include <stdlib.h>
#include <raylib.h>

Game *game_create(void)
{
    // Allocate a single root object that owns long-lived game state.
    Game *game = (Game *)malloc(sizeof(Game));
    if (!game)
        return NULL;

    // Boot into active play state.
    game->state = STATE_RUNNING;
    game->should_close = false;

    // Spawn player near center with configured sprite-sheet settings.
    player_initialize(
        &game->player,
        SCREEN_WIDTH * 0.5f - 48.0f,
        SCREEN_HEIGHT * 0.5f - 48.0f,
        200.0f,
        200.0f,
        PLAYER_TEXTURE_PATH,
        PLAYER_SPRITE_COLUMNS,
        PLAYER_SPRITE_ROWS);

    return game;
}

void game_update(Game *game, float delta_time_seconds)
{
    // Global input that should work even while paused.
    if (IsKeyPressed(KEY_SPACE))
    {
        game->state = (game->state == STATE_RUNNING) ? STATE_PAUSED : STATE_RUNNING;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        game->should_close = true;
    }

    if (game->state != STATE_RUNNING)
        return;

    // Update live simulation: input first, then movement/animation step.
    player_apply_movement_input(&game->player);
    player_update_frame(&game->player, delta_time_seconds);
}

void game_render(Game *game)
{
    // Begin a new frame and clear last frame's color buffer.
    BeginDrawing();
    ClearBackground(RAYWHITE);

    player_render(&game->player);

    // Draw UI overlay last so it appears above gameplay.
    DrawText(TextFormat("Player: (%.0f, %.0f)", game->player.body.position_x, game->player.body.position_y), 10, 10, 20, DARKGRAY);

    if (game->state == STATE_PAUSED)
    {
        DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 40, RED);
    }

    DrawText("WASD/Arrows: Move | Space: Pause | Esc: Quit", 10, SCREEN_HEIGHT - 30, 16, DARKGRAY);

    // Present backbuffer to the screen.
    EndDrawing();
}

void game_destroy(Game *game)
{
    if (game)
    {
        // Player owns texture resources that need explicit unloading.
        player_unload(&game->player);
        free(game);
    }
}
