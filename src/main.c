#include "core/game.h"
#include <raylib.h>
#include <stdio.h>

int main(void)
{
    // Create OS window + graphics context and target frame pacing.
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game Dev in C");
    SetTargetFPS(TARGET_FPS);

    // Build initial game state graph (player, mode, resources).
    Game *game = game_create();
    if (!game)
    {
        fprintf(stderr, "Failed to initialize game\n");
        CloseWindow();
        return 1;
    }

    // Fixed-order frame loop: read dt, update simulation, render frame.
    while (!WindowShouldClose() && !game->should_close)
    {
        // Delta time in seconds since previous frame.
        float delta_time_seconds = GetFrameTime();

        game_update(game, delta_time_seconds);

        game_render(game);
    }

    // Release game-owned resources before shutting down window.
    game_destroy(game);
    CloseWindow();

    return 0;
}
