#include "EntityManager.h"

EntityManager::EntityManager(World &world, Player &player) : _World(world), _Player(player)
{
    _Player.x = _World.Width/2;
    _Player.y = _World.Height/2;
}

EntityManager::~EntityManager()
{
}

void EntityManager::UpdatePlayerPosition()
{
    int &xStart = _Player.xStart;
    int &xEnd = _Player.xEnd;
    int &yStart = _Player.yStart;
    int &yEnd = _Player.yEnd;

    Vec2F velocity = _Player.Velocity;
    float &x = _Player.x, &y = _Player.y;
    float halfSize = _Player.HalfSize, EPSILON = _Player.EPSILON;

    if (velocity.x != 0)
    {
        x += velocity.x;

        xStart = (int)(x - halfSize);
        yStart = (int)(y - halfSize);
        xEnd = (int)(x + halfSize);
        yEnd = (int)(y + halfSize);

        if (xStart < 0)
        {
            x =  halfSize + EPSILON;
        }
        else if (xEnd > _World.Width - 1)
        {
            x = _World.Width - halfSize - EPSILON;
        }
        else
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                //West Collision
                if (_World.tiles[xStart][i] != AIR)
                {
                    x = (float)xStart + 1 + halfSize + EPSILON;
                    break;
                }
                //East Collision
                else if (_World.tiles[xEnd][i] != AIR)
                {
                    x = (float)xEnd - halfSize - EPSILON;
                    break;
                }
            }
        }
    }

    if (velocity.y != 0)
    {
        y += velocity.y;

        xStart = (int)(x - halfSize);
        yStart = (int)(y - halfSize);
        xEnd = (int)(x + halfSize);
        yEnd = (int)(y + halfSize);

        if (yStart < 0)
        {
            y = halfSize + EPSILON;
        }
        else if (yEnd > _World.Height - 1)
        {
            y = _World.Height - halfSize - EPSILON;
        }
        else
        {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (_World.tiles[i][yStart] != AIR)
                {
                    y = (float)yStart + 1 + halfSize + EPSILON;
                    break;
                }
                else if (_World.tiles[i][yEnd] != AIR)
                {
                    y = (float)yEnd - halfSize - EPSILON;
                    break;
                }
            }
        }
    }

    xStart = (int)(x - halfSize);
    yStart = (int)(y - halfSize);
    xEnd = (int)(x + halfSize);
    yEnd = (int)(y + halfSize);
    
    _Player.Velocity = {0, 0};
}