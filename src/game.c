#include "game.h"
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

    // Initialize player entity at center
    entity_init(&game->player,
                SCREEN_WIDTH / 2.0f,
                SCREEN_HEIGHT / 2.0f,
                40.0f, 40.0f);

    return game;
}

void entity_init(Entity *entity, float x, float y, float width, float height)
{
    entity->x = x;
    entity->y = y;
    entity->width = width;
    entity->height = height;
    entity->velocity_x = 0.0f;
    entity->velocity_y = 0.0f;
    entity->active = true;
}

void entity_update(Entity *entity, float dt)
{
    // Update position based on velocity
    entity->x += entity->velocity_x * dt;
    entity->y += entity->velocity_y * dt;

    // Clamp to screen bounds
    if (entity->x < 0)
        entity->x = 0;
    if (entity->x + entity->width > SCREEN_WIDTH)
    {
        entity->x = SCREEN_WIDTH - entity->width;
    }
    if (entity->y < 0)
        entity->y = 0;
    if (entity->y + entity->height > SCREEN_HEIGHT)
    {
        entity->y = SCREEN_HEIGHT - entity->height;
    }
}

void game_update(Game *game, float dt)
{
    // Handle global input first so pause can always be toggled.
    if (IsKeyPressed(KEY_SPACE))
    {
        game->state = (game->state == STATE_RUNNING) ? STATE_PAUSED : STATE_RUNNING;
    }

    if (IsKeyPressed(KEY_Q))
    {
        game->should_close = true;
    }

    if (game->state != STATE_RUNNING)
        return;

    game->elapsed_time += dt;

    // Input handling
    float speed = 300.0f; // pixels per second
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

    // Update entities
    entity_update(&game->player, dt);

    // Game logic goes here
    game->score++;
}

void game_draw(Game *game)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw player
    DrawRectangle(
        (int)game->player.x,
        (int)game->player.y,
        (int)game->player.width,
        (int)game->player.height,
        BLUE);

    // Draw UI
    DrawText("Raylib Game Dev", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Score: %d", game->score), 10, 40, 20, DARKGRAY);
    DrawText(TextFormat("Time: %.1f", game->elapsed_time), 10, 70, 20, DARKGRAY);

    if (game->state == STATE_PAUSED)
    {
        DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 40, RED);
    }

    DrawText("WASD/Arrows: Move | Space: Pause | Q: Quit", 10, SCREEN_HEIGHT - 30, 16, DARKGRAY);

    EndDrawing();
}

void game_cleanup(Game *game)
{
    if (game)
    {
        free(game);
    }
}
