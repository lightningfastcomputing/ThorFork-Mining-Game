#ifndef PLAYER_H
#define PLAYER_H
#include "Types.h"
#include <SDL2/SDL.h>

class Player
{
private:

public:
    const float EPSILON = 0.001;
    SDL_FRect BoundingBox;
    Vec2F HalfDimensions;
    float Speed;
    Vec2F Velocity;
    direction Direction;
    int Score;
    bool CanMine;
    float MiningRadius;

    int xStart, xEnd;
    int yStart, yEnd;

    Player(float width, float height, float speed);
    ~Player();
    void UpdateVelocity(direction dir);
};

#endif
