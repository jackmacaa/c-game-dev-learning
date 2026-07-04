#ifndef ALTAR_H
#define ALTAR_H

#include <stdbool.h>
#include <raylib.h>

typedef struct
{
    Vector2 mountain_center;
    float mountain_radius;
    float interaction_radius;
    bool super_crystal_forged;
} Altar;

// Initialize mountain center and altar interaction radii.
void altar_initialize(Altar *altar, Vector2 mountain_center, float mountain_radius, float interaction_radius);

// Returns true when player center is close enough to interact.
bool altar_is_player_in_range(const Altar *altar, Vector2 player_center);

// Render mountain and altar area in world space.
void altar_render(const Altar *altar);

// Render final crystal when forged.
void altar_render_super_crystal(const Altar *altar);

#endif // ALTAR_H
