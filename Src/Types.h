#ifndef TYPES_H
#define TYPES_H

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

struct Vec2F {
    float x;
    float y;
};

struct Vec2 {
    int x;
    int y;
};

#endif
