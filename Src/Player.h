#ifndef PLAYER_H
#define PLAYER_H
#include "Types.h"
#include <SDL2/SDL.h>

class Player
{
private:

public:
    const float EPSILON = 0.0000000000000000000001f;
    bool **DiscoveredTiles;

    SDL_FRect BoundingBox;
    Vec2F HalfDimensions;
    Vec2F Center;

    float Speed;
    Vec2F Velocity;

    direction Direction;
    
    int Score;
    Vec2F Target;
    bool CanMine;

    float MiningRadius;
    float DiscoverRadius;

    int xStart, xEnd;
    int yStart, yEnd;

    Player(float width, float height, float speed, int worldWidth, int worldHeight);
    ~Player();
    void UpdateVelocity(direction dir);
};

#endif
