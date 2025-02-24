#ifndef TYPES_H
#define TYPES_H

enum Direction
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

struct Vec2F {
    float x;
    float y;
};

#endif
