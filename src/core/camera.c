#include "core/camera.h"
#include <stdlib.h>

static float clampf(float value, float min_value, float max_value)
{
    if (value < min_value)
    {
        return min_value;
    }
    if (value > max_value)
    {
        return max_value;
    }

    return value;
}

GameCamera *camera_create(float viewport_width, float viewport_height)
{
    GameCamera *camera = (GameCamera *)malloc(sizeof(GameCamera));
    if (!camera)
    {
        return NULL;
    }

    camera->viewport_width = viewport_width;
    camera->viewport_height = viewport_height;
    camera->raylib_camera.target = (Vector2){0.0f, 0.0f};
    camera->raylib_camera.offset = (Vector2){viewport_width * 0.5f, viewport_height * 0.5f};
    camera->raylib_camera.rotation = 0.0f;
    camera->raylib_camera.zoom = 1.0f;

    return camera;
}

void camera_update_follow(GameCamera *camera, const World *world, Vector2 target_world_center)
{
    if (!camera || !world)
    {
        return;
    }

    float half_view_width = camera->viewport_width * 0.5f;
    float half_view_height = camera->viewport_height * 0.5f;

    float min_x = half_view_width;
    float min_y = half_view_height;
    float max_x = (float)world->world_width_px - half_view_width;
    float max_y = (float)world->world_height_px - half_view_height;

    if (max_x < min_x)
    {
        float center_x = (float)world->world_width_px * 0.5f;
        min_x = center_x;
        max_x = center_x;
    }

    if (max_y < min_y)
    {
        float center_y = (float)world->world_height_px * 0.5f;
        min_y = center_y;
        max_y = center_y;
    }

    camera->raylib_camera.target.x = clampf(target_world_center.x, min_x, max_x);
    camera->raylib_camera.target.y = clampf(target_world_center.y, min_y, max_y);
}

Camera2D camera_get_raylib(const GameCamera *camera)
{
    if (!camera)
    {
        Camera2D empty_camera = {0};
        return empty_camera;
    }

    return camera->raylib_camera;
}

void camera_destroy(GameCamera *camera)
{
    free(camera);
}
