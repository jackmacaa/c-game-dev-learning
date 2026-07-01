#include "entities/entity.h"
#include "core/config.h"

void entity_init(Entity *entity, float x, float y, float width, float height)
{
    entity->x = x;
    entity->y = y;
    entity->width = width;
    entity->height = height;
    entity->velocity_x = 0.0f;
    entity->velocity_y = 0.0f;
    entity->active = true;
}

void entity_update(Entity *entity, float dt)
{
    entity->x += entity->velocity_x * dt;
    entity->y += entity->velocity_y * dt;

    if (entity->x < 0)
    {
        entity->x = 0;
    }
    if (entity->x + entity->width > SCREEN_WIDTH)
    {
        entity->x = SCREEN_WIDTH - entity->width;
    }
    if (entity->y < 0)
    {
        entity->y = 0;
    }
    if (entity->y + entity->height > SCREEN_HEIGHT)
    {
        entity->y = SCREEN_HEIGHT - entity->height;
    }
}
