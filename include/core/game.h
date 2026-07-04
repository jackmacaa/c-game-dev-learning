#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <stdbool.h>
#include "core/config.h"
#include "entities/player.h"
#include "entities/gem.h"
#include "entities/altar.h"
#include "entities/enemy.h"
#include "core/camera.h"
#include "world/world.h"

#define ENEMY_COUNT 8

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
    World *world;
    GameCamera *camera;
    Gem gems[GEM_COUNT];
    Enemy enemies[ENEMY_COUNT];
    int enemies_alive;
    int collected_gem_count;
    GemType last_collected_gem_type;
    Altar altar;
    bool player_won;
    bool player_lost;
    float attack_cooldown_seconds;
    float attack_feedback_seconds;
    int essence_collected;
    float world_time_seconds;
    bool show_collision_debug;
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
