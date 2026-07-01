#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>

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

void entity_init(Entity *entity, float x, float y, float width, float height);
void entity_update(Entity *entity, float dt);

#endif // ENTITY_H
