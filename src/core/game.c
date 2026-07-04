#include "core/game.h"
#include <stdlib.h>
#include <raylib.h>
#include <math.h>

static Rectangle game_get_player_world_rect(const Game *game)
{
    return (Rectangle){
        game->player.body.position_x,
        game->player.body.position_y,
        game->player.body.render_width,
        game->player.body.render_height};
}

static Vector2 game_get_collision_center_for_body(Rectangle body)
{
    float inset_top = body.height * 0.28f;

    return (Vector2){
        body.x + body.width * 0.5f,
        body.y + inset_top + (body.height - inset_top - body.height * 0.12f) * 0.5f};
}

static float game_get_collision_radius_for_body(Rectangle body)
{
    float inset_x = body.width * 0.30f;
    float inset_top = body.height * 0.28f;
    float inset_bottom = body.height * 0.12f;

    float base_width = body.width - inset_x * 2.0f;
    float base_height = body.height - inset_top - inset_bottom;
    float base_diameter = (base_width < base_height) ? base_width : base_height;

    // Requested gameplay change: shrink the previous hitbox by 50% and use a circle.
    float radius = base_diameter * 0.25f;
    if (radius < 3.0f)
    {
        radius = 3.0f;
    }

    return radius;
}

static bool game_circle_collides_world(const Game *game, Vector2 center, float radius)
{
    return world_circle_collides_blocking_tiles(game->world, center, radius);
}

static bool game_body_collides_world(const Game *game, Rectangle body)
{
    Vector2 center = game_get_collision_center_for_body(body);
    float radius = game_get_collision_radius_for_body(body);
    return game_circle_collides_world(game, center, radius);
}

static Vector2 game_find_nearest_walkable_body_anchor(const Game *game, Vector2 desired_top_left, float width, float height)
{
    Rectangle desired = {desired_top_left.x, desired_top_left.y, width, height};
    world_clamp_rect_to_bounds(game->world, &desired);
    if (!game_body_collides_world(game, desired))
    {
        return (Vector2){desired.x, desired.y};
    }

    int tile_size = game->world->tile_size_px;
    int center_column = (int)((desired.x + desired.width * 0.5f) / (float)tile_size);
    int center_row = (int)((desired.y + desired.height * 0.5f) / (float)tile_size);
    int max_radius = (game->world->columns > game->world->rows) ? game->world->columns : game->world->rows;

    for (int ring = 1; ring <= max_radius; ring++)
    {
        for (int row_offset = -ring; row_offset <= ring; row_offset++)
        {
            for (int column_offset = -ring; column_offset <= ring; column_offset++)
            {
                if (abs(column_offset) != ring && abs(row_offset) != ring)
                {
                    continue;
                }

                int candidate_column = center_column + column_offset;
                int candidate_row = center_row + row_offset;
                if (candidate_column < 0 || candidate_row < 0 || candidate_column >= game->world->columns || candidate_row >= game->world->rows)
                {
                    continue;
                }

                float candidate_center_x = (candidate_column + 0.5f) * (float)tile_size;
                float candidate_center_y = (candidate_row + 0.5f) * (float)tile_size;
                Rectangle candidate = {
                    candidate_center_x - width * 0.5f,
                    candidate_center_y - height * 0.5f,
                    width,
                    height};
                world_clamp_rect_to_bounds(game->world, &candidate);

                if (!game_body_collides_world(game, candidate))
                {
                    return (Vector2){candidate.x, candidate.y};
                }
            }
        }
    }

    return (Vector2){desired.x, desired.y};
}

static Vector2 game_get_player_collision_center(const Game *game)
{
    return game_get_collision_center_for_body(game_get_player_world_rect(game));
}

static float game_get_player_collision_radius(const Game *game)
{
    return game_get_collision_radius_for_body(game_get_player_world_rect(game));
}

static Vector2 game_get_player_center(const Game *game)
{
    return (Vector2){
        game->player.body.position_x + game->player.body.render_width * 0.5f,
        game->player.body.position_y + game->player.body.render_height * 0.5f};
}

static void game_draw_collision_debug(const Game *game)
{
    // Draw blocking terrain tiles so collision boundaries are visible.
    for (int row = 0; row < game->world->rows; row++)
    {
        for (int column = 0; column < game->world->columns; column++)
        {
            TileType tile_type = world_get_tile_type(game->world, column, row);
            if (!world_tile_is_blocking(tile_type))
            {
                continue;
            }

            Rectangle tile_rect = {
                (float)(column * game->world->tile_size_px),
                (float)(row * game->world->tile_size_px),
                (float)game->world->tile_size_px,
                (float)game->world->tile_size_px};

            DrawRectangleRec(tile_rect, (Color){215, 69, 69, 45});
            DrawRectangleLinesEx(tile_rect, 1.2f, (Color){225, 66, 66, 160});
        }
    }

    Rectangle body_rect = game_get_player_world_rect(game);
    Vector2 collision_center = game_get_player_collision_center(game);
    float collision_radius = game_get_player_collision_radius(game);

    // Cyan = visual sprite body box, Yellow = actual collision circle.
    DrawRectangleLinesEx(body_rect, 2.0f, (Color){75, 220, 245, 220});
    DrawCircleLines((int)collision_center.x, (int)collision_center.y, collision_radius, (Color){255, 226, 70, 230});
    DrawCircleV(collision_center, 4.0f, (Color){255, 255, 255, 220});
}

static bool game_can_place_gem_at(const Game *game, float center_x, float center_y, float radius)
{
    Rectangle gem_rect = {
        center_x - radius,
        center_y - radius,
        radius * 2.0f,
        radius * 2.0f};

    world_clamp_rect_to_bounds(game->world, &gem_rect);
    return !world_rect_collides_blocking_tiles(game->world, gem_rect);
}

static void game_relocate_gem_to_walkable(Game *game, Gem *gem)
{
    if (!game || !gem)
    {
        return;
    }

    if (game_can_place_gem_at(game, gem->position.x, gem->position.y, gem->radius))
    {
        return;
    }

    int tile_size = game->world->tile_size_px;
    int base_column = (int)(gem->position.x / (float)tile_size);
    int base_row = (int)(gem->position.y / (float)tile_size);
    int max_radius = (game->world->columns > game->world->rows) ? game->world->columns : game->world->rows;

    for (int ring = 1; ring <= max_radius; ring++)
    {
        for (int row_offset = -ring; row_offset <= ring; row_offset++)
        {
            for (int column_offset = -ring; column_offset <= ring; column_offset++)
            {
                if (abs(column_offset) != ring && abs(row_offset) != ring)
                {
                    continue;
                }

                int candidate_column = base_column + column_offset;
                int candidate_row = base_row + row_offset;
                if (candidate_column < 0 || candidate_row < 0 || candidate_column >= game->world->columns || candidate_row >= game->world->rows)
                {
                    continue;
                }

                float candidate_x = (candidate_column + 0.5f) * (float)tile_size;
                float candidate_y = (candidate_row + 0.5f) * (float)tile_size;
                if (game_can_place_gem_at(game, candidate_x, candidate_y, gem->radius))
                {
                    gem->position = (Vector2){candidate_x, candidate_y};
                    return;
                }
            }
        }
    }
}

static void game_initialize_gems(Game *game)
{
    int margin = 400;
    int max_x = game->world->world_width_px - margin;
    int max_y = game->world->world_height_px - margin;

    gem_initialize(&game->gems[0], GEM_RED, (float)margin, (float)margin, 42.0f);
    gem_initialize(&game->gems[1], GEM_BLUE, (float)max_x, (float)margin + 180.0f, 42.0f);
    gem_initialize(&game->gems[2], GEM_YELLOW, (float)margin + 220.0f, (float)max_y, 42.0f);
    gem_initialize(&game->gems[3], GEM_ORANGE, (float)max_x - 120.0f, (float)max_y - 80.0f, 42.0f);
    gem_initialize(&game->gems[4], GEM_GREEN, (float)game->world->world_width_px * 0.22f, (float)game->world->world_height_px * 0.62f, 42.0f);
    gem_initialize(&game->gems[5], GEM_PURPLE, (float)game->world->world_width_px * 0.78f, (float)game->world->world_height_px * 0.34f, 42.0f);

    for (int gem_index = 0; gem_index < GEM_COUNT; gem_index++)
    {
        game_relocate_gem_to_walkable(game, &game->gems[gem_index]);
    }
}

static void game_initialize_enemies(Game *game)
{
    float patrol_speed = 170.0f;
    float size = 116.0f;

    enemy_initialize(&game->enemies[0], (Vector2){620.0f, 480.0f}, size, (Vector2){1320.0f, 520.0f}, patrol_speed);
    enemy_initialize(&game->enemies[1], (Vector2){3120.0f, 560.0f}, size, (Vector2){3600.0f, 1100.0f}, patrol_speed + 20.0f);
    enemy_initialize(&game->enemies[2], (Vector2){880.0f, 3050.0f}, size, (Vector2){1420.0f, 3500.0f}, patrol_speed + 10.0f);
    enemy_initialize(&game->enemies[3], (Vector2){3300.0f, 3280.0f}, size, (Vector2){3600.0f, 3650.0f}, patrol_speed + 30.0f);
    enemy_initialize(&game->enemies[4], (Vector2){1900.0f, 2320.0f}, size, (Vector2){2300.0f, 2600.0f}, patrol_speed + 5.0f);
    enemy_initialize(&game->enemies[5], (Vector2){2200.0f, 1820.0f}, size, (Vector2){2600.0f, 2100.0f}, patrol_speed + 25.0f);
    enemy_initialize(&game->enemies[6], (Vector2){1780.0f, 1510.0f}, size, (Vector2){2050.0f, 1740.0f}, patrol_speed + 35.0f);
    enemy_initialize(&game->enemies[7], (Vector2){2450.0f, 1480.0f}, size, (Vector2){2740.0f, 1760.0f}, patrol_speed + 15.0f);

    // Move authored points off blocking terrain so patrol starts are always playable.
    for (int enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++)
    {
        Enemy *enemy = &game->enemies[enemy_index];
        float width = enemy->body.render_width;
        float height = enemy->body.render_height;

        Vector2 start = game_find_nearest_walkable_body_anchor(game, enemy->patrol_start, width, height);
        Vector2 patrol_end = game_find_nearest_walkable_body_anchor(game, enemy->patrol_end, width, height);

        enemy->patrol_start = start;
        enemy->patrol_end = patrol_end;
        enemy->body.position_x = start.x;
        enemy->body.position_y = start.y;
    }

    game->enemies_alive = ENEMY_COUNT;
}

static void game_draw_minimap(const Game *game)
{
    float map_width = 230.0f;
    float map_height = 170.0f;
    float map_x = (float)SCREEN_WIDTH - map_width - 18.0f;
    float map_y = 14.0f;

    DrawRectangleRounded((Rectangle){map_x, map_y, map_width, map_height}, 0.09f, 10, (Color){20, 26, 34, 220});
    DrawRectangleLinesEx((Rectangle){map_x, map_y, map_width, map_height}, 1.4f, (Color){130, 156, 179, 240});

    float sx = map_width / (float)game->world->world_width_px;
    float sy = map_height / (float)game->world->world_height_px;

    Vector2 altar_point = {
        map_x + game->altar.mountain_center.x * sx,
        map_y + game->altar.mountain_center.y * sy};
    DrawCircleV(altar_point, 4.0f, (Color){255, 241, 170, 255});

    for (int gem_index = 0; gem_index < GEM_COUNT; gem_index++)
    {
        if (game->gems[gem_index].collected)
        {
            continue;
        }

        Vector2 marker = {
            map_x + game->gems[gem_index].position.x * sx,
            map_y + game->gems[gem_index].position.y * sy};
        DrawCircleV(marker, 3.0f, (Color){74, 225, 245, 255});
    }

    for (int enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++)
    {
        if (!game->enemies[enemy_index].alive)
        {
            continue;
        }

        Vector2 marker = {
            map_x + (game->enemies[enemy_index].body.position_x + game->enemies[enemy_index].body.render_width * 0.5f) * sx,
            map_y + (game->enemies[enemy_index].body.position_y + game->enemies[enemy_index].body.render_height * 0.5f) * sy};
        DrawCircleV(marker, 2.3f, (Color){255, 111, 97, 255});
    }

    Vector2 player_point = {
        map_x + (game->player.body.position_x + game->player.body.render_width * 0.5f) * sx,
        map_y + (game->player.body.position_y + game->player.body.render_height * 0.5f) * sy};
    DrawCircleV(player_point, 4.0f, (Color){151, 248, 171, 255});

    DrawText("MAP", (int)(map_x + 8.0f), (int)(map_y + 6.0f), 14, (Color){200, 218, 235, 255});
}

static void game_update_enemy_ai_and_combat(Game *game, float delta_time_seconds, Vector2 player_collision_center, float player_collision_radius, Vector2 player_center)
{
    for (int enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++)
    {
        Enemy *enemy = &game->enemies[enemy_index];
        if (!enemy->alive)
        {
            continue;
        }

        float previous_x = enemy->body.position_x;
        float previous_y = enemy->body.position_y;

        Vector2 enemy_center = {
            enemy->body.position_x + enemy->body.render_width * 0.5f,
            enemy->body.position_y + enemy->body.render_height * 0.5f};
        float dx = player_center.x - enemy_center.x;
        float dy = player_center.y - enemy_center.y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < 420.0f && distance > 0.001f)
        {
            float direction_x = dx / distance;
            float direction_y = dy / distance;
            float chase_speed = enemy->patrol_speed_units_per_sec * 1.18f;
            enemy->body.position_x += direction_x * chase_speed * delta_time_seconds;
            enemy->body.position_y += direction_y * chase_speed * delta_time_seconds;
        }
        else
        {
            enemy_update(enemy, delta_time_seconds);
        }

        Rectangle enemy_rect = enemy_get_hitbox(enemy);
        world_clamp_rect_to_bounds(game->world, &enemy_rect);
        enemy->body.position_x = enemy_rect.x;
        enemy->body.position_y = enemy_rect.y;

        Vector2 enemy_collision_center = game_get_collision_center_for_body(enemy_rect);
        float enemy_collision_radius = game_get_collision_radius_for_body(enemy_rect);
        if (world_circle_collides_blocking_tiles(game->world, enemy_collision_center, enemy_collision_radius))
        {
            enemy->body.position_x = previous_x;
            enemy->body.position_y = previous_y;
            enemy_update(enemy, delta_time_seconds * 0.35f);
            enemy_rect = enemy_get_hitbox(enemy);
            world_clamp_rect_to_bounds(game->world, &enemy_rect);
            enemy->body.position_x = enemy_rect.x;
            enemy->body.position_y = enemy_rect.y;
            enemy_collision_center = game_get_collision_center_for_body(enemy_rect);
            enemy_collision_radius = game_get_collision_radius_for_body(enemy_rect);

            if (world_circle_collides_blocking_tiles(game->world, enemy_collision_center, enemy_collision_radius))
            {
                enemy->body.position_x = previous_x;
                enemy->body.position_y = previous_y;
                enemy->moving_to_end = !enemy->moving_to_end;
                enemy_rect = enemy_get_hitbox(enemy);
                enemy_collision_center = game_get_collision_center_for_body(enemy_rect);
                enemy_collision_radius = game_get_collision_radius_for_body(enemy_rect);
            }
        }

        if (CheckCollisionCircles(player_collision_center, player_collision_radius, enemy_collision_center, enemy_collision_radius))
        {
            player_apply_damage(&game->player, 1, 1.0f);
        }
    }
}

static void game_reset_run(Game *game)
{
    game->state = STATE_RUNNING;
    game->player_won = false;
    game->player_lost = false;
    game->collected_gem_count = 0;
    game->last_collected_gem_type = GEM_RED;
    game->attack_cooldown_seconds = 0.0f;
    game->attack_feedback_seconds = 0.0f;
    game->essence_collected = 0;
    game->world_time_seconds = 0.0f;
    game->show_collision_debug = false;

    game->player.body.position_x = game->world->player_spawn.x;
    game->player.body.position_y = game->world->player_spawn.y;
    game->player.body.velocity_x = 0.0f;
    game->player.body.velocity_y = 0.0f;
    game->player.health = game->player.max_health;
    game->player.damage_invulnerability_timer = 0.0f;
    game->player.damage_flash_timer = 0.0f;
    game->player.stamina = game->player.max_stamina;
    game->player.stamina_regen_cooldown_seconds = 0.0f;

    altar_initialize(&game->altar, game->world->mountain_center, 130.0f, 72.0f);
    game_initialize_gems(game);
    game_initialize_enemies(game);
    camera_update_follow(game->camera, game->world, game_get_player_center(game));
}

Game *game_create(void)
{
    // Allocate a single root object that owns long-lived game state.
    Game *game = (Game *)malloc(sizeof(Game));
    if (!game)
        return NULL;

    // Boot into active play state.
    game->state = STATE_MENU;
    game->should_close = false;
    game->player_won = false;
    game->player_lost = false;
    game->collected_gem_count = 0;
    game->last_collected_gem_type = GEM_RED;
    game->enemies_alive = 0;
    game->attack_cooldown_seconds = 0.0f;
    game->attack_feedback_seconds = 0.0f;
    game->essence_collected = 0;
    game->world_time_seconds = 0.0f;

    game->world = world_create(WORLD_WIDTH, WORLD_HEIGHT, WORLD_TILE_SIZE);
    if (!game->world)
    {
        free(game);
        return NULL;
    }

    game->camera = camera_create((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    if (!game->camera)
    {
        world_destroy(game->world);
        free(game);
        return NULL;
    }

    // Spawn player near center with configured sprite-sheet settings.
    player_initialize(
        &game->player,
        game->world->player_spawn.x,
        game->world->player_spawn.y,
        PLAYER_RENDER_SIZE,
        PLAYER_RENDER_SIZE,
        PLAYER_TEXTURE_PATH,
        PLAYER_SPRITE_COLUMNS,
        PLAYER_SPRITE_ROWS);

    game_reset_run(game);

    return game;
}

void game_update(Game *game, float delta_time_seconds)
{
    // Global input that should work even while paused.
    if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) && (game->state == STATE_RUNNING || game->state == STATE_PAUSED))
    {
        game->state = (game->state == STATE_RUNNING) ? STATE_PAUSED : STATE_RUNNING;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        game->should_close = true;
    }

    if (IsKeyPressed(KEY_F1))
    {
        game->show_collision_debug = !game->show_collision_debug;
    }

    if (game->state == STATE_GAME_OVER)
    {
        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        {
            game_reset_run(game);
        }
        return;
    }

    if (game->state != STATE_RUNNING)
        return;

    game->world_time_seconds += delta_time_seconds;

    if (game->attack_cooldown_seconds > 0.0f)
    {
        game->attack_cooldown_seconds -= delta_time_seconds;
        if (game->attack_cooldown_seconds < 0.0f)
        {
            game->attack_cooldown_seconds = 0.0f;
        }
    }

    if (game->attack_feedback_seconds > 0.0f)
    {
        game->attack_feedback_seconds -= delta_time_seconds;
        if (game->attack_feedback_seconds < 0.0f)
        {
            game->attack_feedback_seconds = 0.0f;
        }
    }

    // Update live simulation: input first, then movement/animation step.
    float previous_player_x = game->player.body.position_x;
    float previous_player_y = game->player.body.position_y;

    player_apply_movement_input(&game->player);
    player_update_frame(&game->player, delta_time_seconds);

    Rectangle player_body_rect = game_get_player_world_rect(game);
    world_clamp_rect_to_bounds(game->world, &player_body_rect);
    game->player.body.position_x = player_body_rect.x;
    game->player.body.position_y = player_body_rect.y;

    Vector2 player_collision_center = game_get_player_collision_center(game);
    float player_collision_radius = game_get_player_collision_radius(game);

    if (world_circle_collides_blocking_tiles(game->world, player_collision_center, player_collision_radius))
    {
        game->player.body.position_x = previous_player_x;
        game->player.body.position_y = previous_player_y;
        game->player.body.velocity_x = 0.0f;
        game->player.body.velocity_y = 0.0f;
        player_collision_center = game_get_player_collision_center(game);
        player_collision_radius = game_get_player_collision_radius(game);
    }

    Vector2 player_center = game_get_player_center(game);

    if (IsKeyPressed(KEY_SPACE) && game->attack_cooldown_seconds <= 0.0f)
    {
        game->attack_cooldown_seconds = 0.45f;
        game->attack_feedback_seconds = 0.12f;

        float attack_radius = game->player.body.render_width * 0.82f;
        for (int enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++)
        {
            if (enemy_try_take_hit(&game->enemies[enemy_index], player_center, attack_radius))
            {
                game->enemies_alive--;
                game->essence_collected += 3;
            }
        }
    }

    game_update_enemy_ai_and_combat(game, delta_time_seconds, player_collision_center, player_collision_radius, player_center);

    if (player_is_dead(&game->player))
    {
        game->player_lost = true;
        game->state = STATE_GAME_OVER;
        return;
    }

    for (int gem_index = 0; gem_index < GEM_COUNT; gem_index++)
    {
        if (gem_try_collect(&game->gems[gem_index], player_collision_center, player_collision_radius))
        {
            game->collected_gem_count++;
            game->last_collected_gem_type = game->gems[gem_index].type;
        }
    }

    bool player_in_altar_range = altar_is_player_in_range(&game->altar, player_center);
    if (!game->altar.super_crystal_forged && player_in_altar_range && game->collected_gem_count == GEM_COUNT && IsKeyPressed(KEY_E))
    {
        game->altar.super_crystal_forged = true;
        game->player_won = true;
        game->state = STATE_GAME_OVER;
    }

    camera_update_follow(game->camera, game->world, player_center);
}

void game_render(Game *game)
{
    // Begin a new frame and clear last frame's color buffer.
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera_get_raylib(game->camera));

    world_render(game->world);
    altar_render(&game->altar);

    for (int gem_index = 0; gem_index < GEM_COUNT; gem_index++)
    {
        gem_render(&game->gems[gem_index]);
    }

    for (int enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++)
    {
        enemy_render(&game->enemies[enemy_index]);
    }

    player_render(&game->player);

    if (game->attack_feedback_seconds > 0.0f)
    {
        float attack_radius = game->player.body.render_width * 0.82f;
        DrawCircleLines((int)game_get_player_center(game).x, (int)game_get_player_center(game).y, attack_radius, (Color){255, 217, 94, 255});
    }

    if (game->show_collision_debug)
    {
        game_draw_collision_debug(game);
    }

    altar_render_super_crystal(&game->altar);

    EndMode2D();

    // Day-night ambience tint is applied above world but below HUD for readability.
    float cycle = sinf(game->world_time_seconds * 0.045f);
    float night_strength = (cycle + 1.0f) * 0.5f;
    unsigned char alpha = (unsigned char)(40.0f + night_strength * 80.0f);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){22, 32, 54, alpha});

    // Draw UI overlay last so it appears above gameplay.
    DrawText(TextFormat("Player: (%.0f, %.0f)", game->player.body.position_x, game->player.body.position_y), 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("Gems: %d/%d", game->collected_gem_count, GEM_COUNT), 10, 40, 20, BLACK);
    const char *last_gem_name = (game->collected_gem_count > 0) ? gem_type_name(game->last_collected_gem_type) : "None";
    DrawText(TextFormat("Last gem: %s", last_gem_name), 10, 70, 20, DARKBLUE);
    DrawText(TextFormat("Health: %d/%d", game->player.health, game->player.max_health), 10, 100, 20, (Color){138, 18, 18, 255});
    DrawText(TextFormat("Guardians: %d", game->enemies_alive), 10, 130, 20, (Color){122, 47, 33, 255});
    DrawText(TextFormat("Stamina: %d%%", (int)((game->player.stamina / game->player.max_stamina) * 100.0f)), 10, 160, 20, (Color){36, 86, 143, 255});
    DrawText(TextFormat("Essence: %d", game->essence_collected), 10, 190, 20, (Color){90, 50, 140, 255});
    if (game->show_collision_debug)
    {
        DrawText("Collision debug: ON (F1)", 10, 220, 20, (Color){239, 208, 101, 255});
    }
    else
    {
        DrawText("Collision debug: OFF (F1)", 10, 220, 20, (Color){134, 145, 161, 255});
    }

    game_draw_minimap(game);

    if (game->state == STATE_PAUSED)
    {
        DrawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 10, 40, RED);
    }

    bool player_in_altar_range = altar_is_player_in_range(&game->altar, game_get_player_center(game));
    if (!game->altar.super_crystal_forged && player_in_altar_range)
    {
        if (game->collected_gem_count == GEM_COUNT)
        {
            DrawText("Press E at the mountain altar to forge the super crystal", 10, 230, 22, MAROON);
        }
        else
        {
            DrawText("Reach the mountain with all 6 gems", 10, 230, 22, DARKBROWN);
        }
    }

    if (game->player_won)
    {
        DrawText("Super crystal forged. You restored the mountain core!", 10, 260, 24, (Color){33, 94, 129, 255});
        DrawText("Press Enter or R to start a new run", 10, 290, 24, (Color){33, 94, 129, 255});
    }
    else if (game->player_lost)
    {
        DrawText("You were defeated by corrupted guardians.", 10, 260, 24, (Color){124, 21, 21, 255});
        DrawText("Press Enter or R to restart and reclaim the crystals", 10, 290, 24, (Color){124, 21, 21, 255});
    }

    DrawText("WASD/Arrows: Move | Shift: Sprint | Space: Strike | E: Forge | F1: Hitboxes | Enter: Pause | Esc: Quit", 10, SCREEN_HEIGHT - 30, 16, DARKGRAY);

    // Present backbuffer to the screen.
    EndDrawing();
}

void game_destroy(Game *game)
{
    if (game)
    {
        // Player owns texture resources that need explicit unloading.
        player_unload(&game->player);
        camera_destroy(game->camera);
        world_destroy(game->world);
        free(game);
    }
}
