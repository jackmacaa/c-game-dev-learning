#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>

typedef struct
{
    float position_x;
    float position_y;
    float velocity_x;
    float velocity_y;
    float render_width;
    float render_height;
    bool active;
} Entity;

// Initialize an entity's transform and runtime flags.
void entity_initialize(Entity *entity, float start_position_x, float start_position_y, float render_width, float render_height);

// Integrate velocity and keep the entity inside the screen bounds.
void entity_update_kinematics(Entity *entity, float delta_time_seconds);

#endif // ENTITY_H
