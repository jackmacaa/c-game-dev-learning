#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

// Game dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TARGET_FPS 60

// Game state
typedef enum
{
    STATE_MENU,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_GAME_OVER
} GameState;

// Simple entity/actor system
typedef struct
{
    float x;
    float y;
    float velocity_x;
    float velocity_y;
    float width;
    float height;
    bool active;
} Entity;

// Game context - holds all game state
typedef struct
{
    GameState state;
    Entity player;
    float elapsed_time;
    int score;
    bool should_close;
} Game;

// Game lifecycle functions
Game *game_init(void);
void game_update(Game *game, float dt);
void game_draw(Game *game);
void game_cleanup(Game *game);

// Helper functions
void entity_init(Entity *entity, float x, float y, float width, float height);
void entity_update(Entity *entity, float dt);

#endif // GAME_H
