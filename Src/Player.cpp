#include "Player.h"

Player::Player(float width, float height, float speed, int worldWidth, int worldHeight)
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
    DiscoverRadius = 8.0f;

    //tiles inhabited
    xStart = -1;
    xEnd = -1;
    yStart = -1;
    yEnd = -1;
    
    DiscoveredTiles = new bool*[worldWidth];
    for (int i = 0; i < worldWidth; i++)
    {
        DiscoveredTiles[i] = new bool[worldHeight];
        memset(DiscoveredTiles[i], false, worldHeight * sizeof(bool));
    }
}

Player::~Player() //fix memory leak
{
    delete[] DiscoveredTiles;
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