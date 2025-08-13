#include "Player.h"

Player::Player(float width, float height, float speed) : Entity(100, 100, width, height)
{
    Speed = speed; // tiles per tick, dont go too fast
    Direction = NONE;
    Score = 0;
    CanMine = false;
    Target = Center;
    MiningRadius = 3.0f;
    DiscoverRadius = 50.0f;
    MaxVelocity = 0.15f;
    DragCoefficient = 0.15f;

    Elastic = false;
    Corporeal = true;

    xStart = -1;
    yStart = -1;
    xEnd = -1;
    yEnd = -1;

    type = PLAYER;
    InteractMode = false;

}

Player::~Player()
{
}

void Player::UpdateAcceleration(direction dir)
{
    Direction = dir;
    switch (dir)
    {
    case WEST:
        Acceleration = {-Speed, 0};
        break;
    case EAST:
        Acceleration = {Speed, 0};
        break;
    case NORTH:
        Acceleration = {0, -Speed};
        break;
    case SOUTH:
        Acceleration = {0, Speed};
        break;
    case NORTHWEST:
        Acceleration = {-Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case NORTHEAST:
        Acceleration = {Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case SOUTHWEST:
        Acceleration = {-Speed * 0.7071f, Speed * 0.7071f};
        break;
    case SOUTHEAST:
        Acceleration = {Speed * 0.7071f, Speed * 0.7071f};
        break;
    case NONE:
        Acceleration = {0, 0};
    }
}