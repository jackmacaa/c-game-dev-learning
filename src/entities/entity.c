#include "entities/entity.h"

void entity_initialize(Entity *entity, float start_position_x, float start_position_y, float render_width, float render_height)
{
    // Initial transform and movement state for a newly created entity.
    entity->position_x = start_position_x;
    entity->position_y = start_position_y;
    entity->render_width = render_width;
    entity->render_height = render_height;
    entity->velocity_x = 0.0f;
    entity->velocity_y = 0.0f;
    entity->active = true;
}

void entity_update_kinematics(Entity *entity, float delta_time_seconds)
{
    // Integrate velocity into position using frame delta time.
    entity->position_x += entity->velocity_x * delta_time_seconds;
    entity->position_y += entity->velocity_y * delta_time_seconds;
}
