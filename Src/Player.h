#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <stdio.h>
#include "World.h"
#include <SDL2/SDL.h>

typedef enum direction_t
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
} direction;

struct Vec2F {
    float x;
    float y;
};

class Player
{
private:
    World *_World;
public:
    float Size;
    float x, y;
    float Speed;
    direction Direction;
    int Score;
    bool CanMine;
    float MiningRadius;

    Player(World *World);
    ~Player();
    void TryMove(direction dir);
};

#endif
