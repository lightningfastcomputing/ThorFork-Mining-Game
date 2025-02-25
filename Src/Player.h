#ifndef PLAYER_H
#define PLAYER_H
#include "Types.h"

class Player
{
private:

public:
    const float EPSILON = 0.001;
    float Size;
    float x, y;
    float Speed;
    Vec2F Velocity;
    direction Direction;
    int Score;
    bool CanMine;
    float MiningRadius;

    Player();
    ~Player();
    void UpdateVelocity(direction dir);
};

#endif
