#include "entities/altar.h"

void altar_initialize(Altar *altar, Vector2 mountain_center, float mountain_radius, float interaction_radius)
{
    if (!altar)
    {
        return;
    }

    altar->mountain_center = mountain_center;
    altar->mountain_radius = mountain_radius;
    altar->interaction_radius = interaction_radius;
    altar->super_crystal_forged = false;
}

bool altar_is_player_in_range(const Altar *altar, Vector2 player_center)
{
    if (!altar)
    {
        return false;
    }

    return CheckCollisionPointCircle(player_center, altar->mountain_center, altar->interaction_radius);
}

void altar_render(const Altar *altar)
{
    if (!altar)
    {
        return;
    }

    DrawCircleV(altar->mountain_center, altar->mountain_radius, (Color){120, 112, 104, 255});
    DrawCircleV(altar->mountain_center, altar->mountain_radius * 0.72f, (Color){146, 136, 124, 255});

    DrawCircleV(altar->mountain_center, altar->interaction_radius, (Color){226, 216, 182, 255});
    DrawCircleLines((int)altar->mountain_center.x, (int)altar->mountain_center.y, altar->interaction_radius, (Color){95, 79, 59, 220});
}

void altar_render_super_crystal(const Altar *altar)
{
    if (!altar || !altar->super_crystal_forged)
    {
        return;
    }

    float size = altar->interaction_radius * 0.9f;
    Vector2 top = {altar->mountain_center.x, altar->mountain_center.y - size};
    Vector2 right = {altar->mountain_center.x + size * 0.72f, altar->mountain_center.y};
    Vector2 bottom = {altar->mountain_center.x, altar->mountain_center.y + size};
    Vector2 left = {altar->mountain_center.x - size * 0.72f, altar->mountain_center.y};

    // Layered glow makes the forged crystal feel like a high-value reward.
    DrawCircleV(altar->mountain_center, size * 0.95f, (Color){255, 245, 170, 60});
    DrawCircleV(altar->mountain_center, size * 0.72f, (Color){165, 225, 255, 90});

    Vector2 core = altar->mountain_center;
    DrawTriangle(top, core, left, (Color){255, 95, 90, 235});
    DrawTriangle(top, right, core, (Color){254, 211, 86, 235});
    DrawTriangle(core, right, bottom, (Color){84, 204, 124, 235});
    DrawTriangle(left, core, bottom, (Color){87, 153, 255, 235});

    DrawTriangle((Vector2){core.x, core.y - size * 0.62f},
                 (Vector2){core.x + size * 0.20f, core.y - size * 0.08f},
                 (Vector2){core.x - size * 0.12f, core.y + size * 0.02f},
                 (Color){255, 255, 255, 190});

    DrawTriangleLines(top, right, left, (Color){39, 65, 96, 255});
    DrawTriangleLines(bottom, right, left, (Color){39, 65, 96, 255});
}
