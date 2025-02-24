#include "Player.h"

Player::Player()
{
    this->x = 0;
    this->y = 0;
    Speed = 0.1f;               //tiles per tick, 0 < speed < 1;
    Size = 0.6f;                //0 < size < 1;
    Velocity = {0, 0};                
    Direction = NONE;
    Score = 0;
    CanMine = false;
    MiningRadius = 2.0f;

}

Player::~Player()
{
}

void Player::UpdateVelocity(Direction dir)
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