#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <raylib.h>
#include "world/world.h"

typedef struct
{
    Camera2D raylib_camera;
    float viewport_width;
    float viewport_height;
} GameCamera;

// Create a 2D camera with a fixed viewport and default zoom.
GameCamera *camera_create(float viewport_width, float viewport_height);

// Follow a world-space target while clamping view to world edges.
void camera_update_follow(GameCamera *camera, const World *world, Vector2 target_world_center);

// Access raylib camera object for BeginMode2D.
Camera2D camera_get_raylib(const GameCamera *camera);

// Release camera heap memory.
void camera_destroy(GameCamera *camera);

#endif // GAME_CAMERA_H
