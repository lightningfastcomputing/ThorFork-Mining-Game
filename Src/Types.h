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

enum Tile
{
    AIR,
    STONE,
    GOLD,
    EXPLOSIVE,
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

    float Magnitude()
    {
        return (SDL_sqrtf((x * x) + (y * y)));
    }

    void Normalize()
    {
        float invMagnitude = 1 / (Magnitude());
        x *= invMagnitude;
        y *= invMagnitude;
    }

    Vec2F operator+(const Vec2F &other) const
    {
        return {x + other.x, y + other.y};
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
    Vec2F Direction;
    float Length;

    Ray(Vec2F origin, Vec2F dir, float length)
    {
        Origin = origin;
        Direction = dir;
        dir.Normalize();
        Length = length;
    }

    Vec2F Endpoint()
    {
        return {Origin.x + Direction.x * Length, Origin.y + Direction.y * Length};
    }

    std::vector<Vec2> GetTiles()
    {
        std::vector<Vec2> tiles;
        Vec2F rayPos = Origin;

        for (float t = 0; t < Length; t += 0.01f)
        {
            rayPos = Origin + Direction * t;

            Vec2 tile = {(int)rayPos.x, (int)rayPos.y};

            if (tiles.empty() || tiles.back().x != tile.x || tiles.back().y != tile.y)
            {
                tiles.push_back(tile);
            }
        }

        return tiles;
    }
};

#endif
