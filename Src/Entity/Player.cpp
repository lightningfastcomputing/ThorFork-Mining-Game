#include "Player.h"

Player::Player(float width, float height, float speed) : Entity(100, 100, width, height)
{
    Speed = speed; // tiles per tick, dont go too fast
    Direction = NONE;
    Score = 0;
    CanMine = false;
    Target = Center;
    MiningRadius = 5.0f;
    DiscoverRadius = 50.0f;
    MaxVelocity = 0.15f;
    DragCoefficient = 0.15f;

    Elastic = false;
    Killable = false;

    xStart = -1;
    yStart = -1;
    xEnd = -1;
    yEnd = -1;

    Type = PLAYER;
    CollisionType = 1 << Type;
    // Collides = 1 << PLAYER | 1 << CHUNK | 1 << MINECART | 1 << DYNAMITE;

    InteractMode = false;
}

Player::~Player()
{
}

void Player::Interact()
{

}

void Player::UpdateAcceleration(direction dir)
{
    Vec2F &a = Parent ? Parent->Acceleration : Acceleration; 
    Direction = dir;
    switch (dir)
    {
    case WEST:
        a = {-Speed, 0};
        break;
    case EAST:
        a = {Speed, 0};
        break;
    case NORTH:
        a = {0, -Speed};
        break;
    case SOUTH:
        a = {0, Speed};
        break;
    case NORTHWEST:
        a = {-Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case NORTHEAST:
        a = {Speed * 0.7071f, -Speed * 0.7071f};
        break;
    case SOUTHWEST:
        a = {-Speed * 0.7071f, Speed * 0.7071f};
        break;
    case SOUTHEAST:
        a = {Speed * 0.7071f, Speed * 0.7071f};
        break;
    case NONE:
        a = {0, 0};
    }
}

std::string Player::DebugInfo()
{
    return "PLAYER\n";
}