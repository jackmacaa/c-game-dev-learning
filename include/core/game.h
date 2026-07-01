#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <stdbool.h>
#include "core/config.h"
#include "entities/player.h"

typedef enum
{
    STATE_MENU,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

typedef struct
{
    GameState state;
    Player player;
    bool should_close;
} Game;

// Allocate and initialize the main game state.
Game *game_create(void);

// Run one simulation step using frame delta time in seconds.
void game_update(Game *game, float delta_time_seconds);

// Render one complete frame.
void game_render(Game *game);

// Free game state and owned resources.
void game_destroy(Game *game);

#endif // GAME_CORE_H
