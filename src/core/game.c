#include "core/game.h"
#include <stdlib.h>
#include <raylib.h>

Game *game_init(void)
{
    Game *game = (Game *)malloc(sizeof(Game));
    if (!game)
        return NULL;

    game->state = STATE_RUNNING;
    game->elapsed_time = 0.0f;
    game->score = 0;
    game->should_close = false;

    entity_init(&game->player,
                SCREEN_WIDTH / 2.0f,
                SCREEN_HEIGHT / 2.0f,
                40.0f, 40.0f);

    return game;
}

void game_update(Game *game, float dt)
{
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

    game->elapsed_time += dt;

    float speed = 300.0f;
    game->player.velocity_x = 0.0f;
    game->player.velocity_y = 0.0f;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        game->player.velocity_x = speed;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        game->player.velocity_x = -speed;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        game->player.velocity_y = speed;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        game->player.velocity_y = -speed;
    }

    entity_update(&game->player, dt);
    game->score++;
}

void game_draw(Game *game)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawCircle(
        (int)game->player.x,
        (int)game->player.y,
        (int)(game->player.width / 2),
        BLUE);

    DrawText("Raylib Game Dev", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Score: %d", game->score), 10, 40, 20, DARKGRAY);
    DrawText(TextFormat("Time: %.1f", game->elapsed_time), 10, 70, 20, DARKGRAY);

    if (game->state == STATE_PAUSED)
    {
        DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 40, RED);
    }

    DrawText("WASD/Arrows: Move | Space: Pause | Esc: Quit", 10, SCREEN_HEIGHT - 30, 16, DARKGRAY);

    EndDrawing();
}

void game_cleanup(Game *game)
{
    if (game)
    {
        free(game);
    }
}
