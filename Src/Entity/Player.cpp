#include "Player.h"

Player::Player(float width, float height, float speed) : Entity(100, 100, width, height)
{
    Speed = speed; // tiles per tick, dont go too fast
    Direction = NONE;
    Score = 0;
    CanMine = false;
    Target = Center;
    MiningRadius = 10.0f;
    DiscoverRadius = 50.0f;

    xStart = -1;
    yStart = -1;
    xEnd = -1;
    yEnd = -1;

    type = PLAYER;
}

Player::~Player()
{
}

void Player::UpdateVelocity(direction dir)
{
    this->Direction = dir;
    switch (dir)
    {
    case WEST:
        Velocity = {-Speed, 0};
        break;
    case EAST:
        Velocity = {Speed, 0};
        break;
    case NORTH:
        Velocity = {0, -Speed};
        break;
    case SOUTH:
        Velocity = {0, Speed};
        break;
    case NORTHWEST:
        Velocity = {-Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case NORTHEAST:
        Velocity = {Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case SOUTHWEST:
        Velocity = {-Speed * 0.7071f, Speed * 0.7071f};
        break;
    case SOUTHEAST:
        Velocity = {Speed * 0.7071f, Speed * 0.7071f};
        break;
    case NONE:
        Velocity = {0, 0};
    }
}