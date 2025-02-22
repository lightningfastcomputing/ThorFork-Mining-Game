#include "Player.h"
#include <stdio.h>

#define SQRT2INV 0.7071f; //diagonal movements
#define EPSILON 0.001f; //collision tolerance due to floating point precision errors: collisions to the right will not work if you don't use epsilon
Player::Player(World *world)
{
    this->_World = world;
    this->x = _World->Width / 2;
    this->y = _World->Height / 2;
    Speed = 0.1f;               //tiles per tick, 0 < speed < 1;
    Size = 0.6f;                //0 < size < 1;
    Direction = NONE;
    Score = 0;
    CanMine = false;
    MiningRadius = 2.0f;

}

Player::~Player()
{
}

void Player::TryMove(direction dir)
{
    struct Vec2F velocity;
    this->Direction = dir;
    switch (dir)
    {
    case WEST:
        velocity = {-Speed, 0};
        break;
    case EAST:
        velocity = {Speed, 0};
        break;
    case NORTH:
        velocity = {0, -Speed};
        break;
    case SOUTH:
        velocity = {0, Speed};
        break;
    case NORTHWEST:
        velocity = {-Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case NORTHEAST:
        velocity = {Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case SOUTHWEST:
        velocity = {-Speed * 0.7071f, Speed * 0.7071f};
        break;
    case SOUTHEAST:
        velocity = {Speed * 0.7071f, Speed * 0.7071f};
        break;
    }
    bool NW, NE, SW, SE;
    int xStart, yStart, xEnd, yEnd;

    if (velocity.x != 0)
    {
        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + Size);
        yEnd = (int)SDL_floorf(y + Size);

        if (xStart < 0)
        {
            x = EPSILON;
        }
        else if (xEnd > _World->Width - 1)
        {
            x = _World->Width - Size - EPSILON;
        }
        else
        {

            NW = _World->tiles[xStart][yStart] != AIR;
            NE = _World->tiles[xEnd][yStart] != AIR;
            SW = _World->tiles[xStart][yEnd] != AIR;
            SE = _World->tiles[xEnd][yEnd] != AIR;

            if (NW || SW)
            {
                x = (float)xStart + 1 + EPSILON;
            }
            else if (NE || SE)
            {
                x = (float)xEnd - Size - EPSILON;
            }
        }
    }

    if (velocity.y != 0)
    {
        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + Size);
        yEnd = (int)SDL_floorf(y + Size);

        if (yStart < 0)
        {
            y = EPSILON;
        }
        else if (yEnd > _World->Height - 1)
        {
            y = _World->Height - Size - EPSILON;
        }
        else
        {
            NW = _World->tiles[xStart][yStart] != AIR;
            NE = _World->tiles[xEnd][yStart] != AIR;
            SW = _World->tiles[xStart][yEnd] != AIR;
            SE = _World->tiles[xEnd][yEnd] != AIR;

            if (NW || NE)
            {
                y = (float)yStart + 1 + EPSILON;
            }
            else if (SW || SE)
            {
                y = (float)yEnd - Size - EPSILON;
            }
        }
    }
}