#include "Player.h"

Player::Player(float width, float height, float speed)
{
    BoundingBox.x = 0;
    BoundingBox.y = 0;
    BoundingBox.w = width;
    BoundingBox.h = height;
    HalfDimensions = {BoundingBox.w/2, BoundingBox.h/2};
    Center = {BoundingBox.x + HalfDimensions.x, BoundingBox.y + HalfDimensions.y};
    Speed = speed; // tiles per tick, dont go too fast
    Velocity = {0, 0};
    Direction = NONE;
    Score = 0;
    CanMine = false;
    Target = {0, 0};
    MiningRadius = 10.0f;
    DiscoverRadius = 15.0f;

    //tiles inhabited
    xStart = 0;
    xEnd = 0;
    yStart = 0;
    yEnd = 0;
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