#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <stdbool.h>
#include "core/config.h"
#include "entities/entity.h"

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
    Entity player;
    float elapsed_time;
    int score;
    bool should_close;
} Game;

Game *game_init(void);
void game_update(Game *game, float dt);
void game_draw(Game *game);
void game_cleanup(Game *game);

#endif // GAME_CORE_H
