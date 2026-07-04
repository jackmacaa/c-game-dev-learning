#include "entities/enemy.h"
#include <math.h>

void enemy_initialize(Enemy *enemy, Vector2 start_position, float size, Vector2 patrol_end, float patrol_speed_units_per_sec)
{
    if (!enemy)
    {
        return;
    }

    entity_initialize(&enemy->body, start_position.x, start_position.y, size, size);
    enemy->patrol_start = start_position;
    enemy->patrol_end = patrol_end;
    enemy->patrol_speed_units_per_sec = patrol_speed_units_per_sec;
    enemy->moving_to_end = true;
    enemy->alive = true;
}

Rectangle enemy_get_hitbox(const Enemy *enemy)
{
    if (!enemy || !enemy->alive)
    {
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};
    }

    return (Rectangle){
        enemy->body.position_x,
        enemy->body.position_y,
        enemy->body.render_width,
        enemy->body.render_height};
}

void enemy_update(Enemy *enemy, float delta_time_seconds)
{
    if (!enemy || !enemy->alive)
    {
        return;
    }

    Vector2 current = {enemy->body.position_x, enemy->body.position_y};
    Vector2 target = enemy->moving_to_end ? enemy->patrol_end : enemy->patrol_start;

    float to_target_x = target.x - current.x;
    float to_target_y = target.y - current.y;
    float distance = sqrtf(to_target_x * to_target_x + to_target_y * to_target_y);

    if (distance < 4.0f)
    {
        enemy->moving_to_end = !enemy->moving_to_end;
        return;
    }

    float direction_x = to_target_x / distance;
    float direction_y = to_target_y / distance;

    enemy->body.position_x += direction_x * enemy->patrol_speed_units_per_sec * delta_time_seconds;
    enemy->body.position_y += direction_y * enemy->patrol_speed_units_per_sec * delta_time_seconds;
}

bool enemy_try_take_hit(Enemy *enemy, Vector2 attack_center, float attack_radius)
{
    if (!enemy || !enemy->alive)
    {
        return false;
    }

    Rectangle hitbox = enemy_get_hitbox(enemy);
    if (CheckCollisionCircleRec(attack_center, attack_radius, hitbox))
    {
        enemy->alive = false;
        return true;
    }

    return false;
}

void enemy_render(const Enemy *enemy)
{
    if (!enemy || !enemy->alive)
    {
        return;
    }

    Rectangle body = enemy_get_hitbox(enemy);
    float center_x = body.x + body.width * 0.5f;
    float center_y = body.y + body.height * 0.5f;

    DrawRectangleRounded(body, 0.25f, 8, (Color){136, 62, 52, 255});

    Rectangle chest = {
        body.x + body.width * 0.20f,
        body.y + body.height * 0.25f,
        body.width * 0.60f,
        body.height * 0.50f};
    DrawRectangleRounded(chest, 0.18f, 6, (Color){184, 89, 73, 255});

    DrawCircleV((Vector2){center_x - body.width * 0.18f, center_y - body.height * 0.10f}, body.width * 0.08f, (Color){255, 220, 120, 255});
    DrawCircleV((Vector2){center_x + body.width * 0.18f, center_y - body.height * 0.10f}, body.width * 0.08f, (Color){255, 220, 120, 255});

    DrawLineEx((Vector2){body.x + body.width * 0.2f, body.y + body.height * 0.75f},
               (Vector2){body.x + body.width * 0.8f, body.y + body.height * 0.75f},
               2.5f,
               (Color){72, 20, 20, 210});

    DrawCircleLines((int)center_x, (int)center_y, body.width * 0.52f, (Color){70, 20, 20, 120});
}
