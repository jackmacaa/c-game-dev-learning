#include "entities/gem.h"

static Color color_scale(Color color, float scale)
{
    int r = (int)((float)color.r * scale);
    int g = (int)((float)color.g * scale);
    int b = (int)((float)color.b * scale);

    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;

    return (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, color.a};
}

static Color gem_type_color(GemType type)
{
    switch (type)
    {
    case GEM_RED:
        return (Color){228, 79, 74, 255};
    case GEM_BLUE:
        return (Color){62, 133, 238, 255};
    case GEM_YELLOW:
        return (Color){242, 199, 77, 255};
    case GEM_ORANGE:
        return (Color){234, 146, 59, 255};
    case GEM_GREEN:
        return (Color){83, 176, 96, 255};
    case GEM_PURPLE:
        return (Color){154, 101, 214, 255};
    default:
        return LIGHTGRAY;
    }
}

const char *gem_type_name(GemType type)
{
    switch (type)
    {
    case GEM_RED:
        return "Red";
    case GEM_BLUE:
        return "Blue";
    case GEM_YELLOW:
        return "Yellow";
    case GEM_ORANGE:
        return "Orange";
    case GEM_GREEN:
        return "Green";
    case GEM_PURPLE:
        return "Purple";
    default:
        return "Unknown";
    }
}

void gem_initialize(Gem *gem, GemType type, float x, float y, float radius)
{
    if (!gem)
    {
        return;
    }

    gem->type = type;
    gem->position = (Vector2){x, y};
    gem->radius = radius;
    gem->collected = false;
}

bool gem_try_collect(Gem *gem, Vector2 player_collision_center, float player_collision_radius)
{
    if (!gem || gem->collected)
    {
        return false;
    }

    if (CheckCollisionCircles(gem->position, gem->radius, player_collision_center, player_collision_radius))
    {
        gem->collected = true;
        return true;
    }

    return false;
}

void gem_render(const Gem *gem)
{
    if (!gem || gem->collected)
    {
        return;
    }

    Color base = gem_type_color(gem->type);
    Color bright = color_scale(base, 1.25f);
    Color dark = color_scale(base, 0.70f);
    Color aura = base;
    aura.a = 70;

    float radius = gem->radius;
    Vector2 top = {gem->position.x, gem->position.y - radius};
    Vector2 left = {gem->position.x - radius * 0.86f, gem->position.y + radius * 0.5f};
    Vector2 right = {gem->position.x + radius * 0.86f, gem->position.y + radius * 0.5f};
    Vector2 mid = {gem->position.x, gem->position.y + radius * 0.22f};
    Vector2 highlight = {gem->position.x - radius * 0.20f, gem->position.y - radius * 0.22f};

    // Soft glow to make crystals pop against terrain colors.
    DrawCircleV(gem->position, radius * 1.18f, aura);

    // Split into two facets for a crystal-like read.
    DrawTriangle(top, mid, left, bright);
    DrawTriangle(top, right, mid, dark);

    // Inner facet and highlight streak.
    DrawTriangle((Vector2){gem->position.x, gem->position.y - radius * 0.55f},
                 (Vector2){gem->position.x + radius * 0.26f, gem->position.y + radius * 0.10f},
                 (Vector2){gem->position.x - radius * 0.18f, gem->position.y + radius * 0.05f},
                 (Color){245, 250, 255, 180});
    DrawLineEx(highlight, (Vector2){highlight.x + radius * 0.40f, highlight.y + radius * 0.75f}, 2.0f, (Color){255, 255, 255, 170});

    DrawTriangleLines(top, right, left, (Color){28, 30, 35, 255});
}
