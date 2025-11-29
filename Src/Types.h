#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <cstring>
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
    STONE_FLOOR,
    STONE,
    DENSE_STONE,
    GOLD,
    BARRIER,
    TILETYPE_COUNT
};

enum EntityType
{
    MINECART_TRACK,
    MINECART,
    PLAYER,
    CHUNK,
    DYNAMITE,
    ENTITYTYPE_COUNT
};

struct Vec2F;

struct Vec2
{
    int x;
    int y;

    Vec2F ToVec2F() const;
    Vec2 operator+(const Vec2 &other) const;
    Vec2 operator-(const Vec2 &other) const;
    Vec2 operator*(int scalar) const;
    Vec2 operator/(int scalar) const;
    void operator+=(const Vec2 &other);
    bool operator==(const Vec2 &other)
    {
        return (x == other.x) && (y == other.y);
    }
};

struct Vec2F
{
    float x;
    float y;

    float Magnitude() const;
    void Normalize();
    Vec2 ToVec2() const;
    Vec2F Floor() const;
    Vec2F Trunc() const;
    Vec2F operator+(const Vec2F &other) const;
    Vec2F operator-(const Vec2F &other) const;
    Vec2F operator*(float scalar) const;
    Vec2F operator/(float scalar) const;
    void operator+=(const Vec2F &other);
};

#endif
