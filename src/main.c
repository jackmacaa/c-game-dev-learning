#include "game.h"
#include <raylib.h>
#include <stdio.h>

int main(void)
{
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Game Dev");
    SetTargetFPS(TARGET_FPS);

    // Initialize game
    Game *game = game_init();
    if (!game)
    {
        fprintf(stderr, "Failed to initialize game\n");
        CloseWindow();
        return 1;
    }

    // Main game loop
    while (!WindowShouldClose() && !game->should_close)
    {
        // Calculate delta time
        float dt = GetFrameTime();

        // Update
        game_update(game, dt);

        // Draw
        game_draw(game);
    }

    // Cleanup
    game_cleanup(game);
    CloseWindow();

    return 0;
}
