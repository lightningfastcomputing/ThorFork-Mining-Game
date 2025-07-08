#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <vector>

#define PI 3.1415926f

enum direction
{
    WEST,
    EAST,
    NORTH,
    SOUTH,
    NORTHWEST,
    NORTHEAST,
    SOUTHWEST,
    SOUTHEAST,
    NONE
};


enum TileType
{
    AIR,
    STONE,
    DENSE_STONE,
    GOLD,
    EXPLOSIVE,
    BARRIER,
    TILETYPE_COUNT
};

struct Vec2
{
    int x;
    int y;
};

struct Vec2F
{
    float x;
    float y;

    float Magnitude() const
    {
        return (SDL_sqrtf((x * x) + (y * y)));
    }

    void Normalize()
    {
        float magnitude = this->Magnitude();
        if (magnitude != 0)
        {
            float invMagnitude = 1 / (magnitude);
            x *= invMagnitude;
            y *= invMagnitude;
        }
    }

    Vec2F operator+(const Vec2F &other) const
    {
        return {x + other.x, y + other.y};
    }

    Vec2F operator-(const Vec2F &other) const
    {
        return {x - other.x, y - other.y};
    }

    Vec2F operator*(float scalar) const
    {
        return {x * scalar, y * scalar};
    }

    void operator+=(const Vec2F &other)
    {
        x += other.x;
        y += other.y;
    }
};

struct Ray
{
    Vec2F Origin;
    float Angle;
    float Length;

    Ray(const Vec2F &origin, float angle, float length)
    {
        Origin = origin;
        Angle = angle;
        Length = length;
    }
};

#endif
