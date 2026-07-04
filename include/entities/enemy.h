#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include <raylib.h>
#include "entities/entity.h"

typedef struct
{
    Entity body;
    Vector2 patrol_start;
    Vector2 patrol_end;
    float patrol_speed_units_per_sec;
    bool moving_to_end;
    bool alive;
} Enemy;

// Spawn an enemy that patrols between two world-space points.
void enemy_initialize(Enemy *enemy, Vector2 start_position, float size, Vector2 patrol_end, float patrol_speed_units_per_sec);

// Move enemy along its patrol route.
void enemy_update(Enemy *enemy, float delta_time_seconds);

// Render enemy with layered pseudo-texture shapes.
void enemy_render(const Enemy *enemy);

// Read-only world hitbox for overlap checks.
Rectangle enemy_get_hitbox(const Enemy *enemy);

// Return true when player attack circle intersects enemy.
bool enemy_try_take_hit(Enemy *enemy, Vector2 attack_center, float attack_radius);

#endif // ENEMY_H
