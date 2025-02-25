#include "EntityManager.h"

EntityManager::EntityManager(World &world, Player &player) : _World(world), _Player(player)
{
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
    float size = _Player.Size, EPSILON = _Player.EPSILON;

    if (velocity.x != 0)
    {
        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + size);
        yEnd = (int)SDL_floorf(y + size);

        if (xStart < 0)
        {
            x = EPSILON;
        }
        else if (xEnd > _World.Width - 1)
        {
            x = _World.Width - size - EPSILON;
        }
        else
        {
            // West and East Collisions
            for (int i = yStart; i <= yEnd; i++)
            {
                // West
                if (_World.tiles[xStart][i] != AIR)
                {
                    x = (float)xStart + 1 + EPSILON;
                    break;
                }
                // East
                else if (_World.tiles[xEnd][i] != AIR)
                {
                    x = (float)xEnd - size - EPSILON;
                    break;
                }
            }
        }
    }

    if (velocity.y != 0)
    {
        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + size);
        yEnd = (int)SDL_floorf(y + size);

        if (yStart < 0)
        {
            y = EPSILON;
        }
        else if (yEnd > _World.Height - 1)
        {
            y = _World.Height - size - EPSILON;
        }
        else
        {
            for (int i = xStart; i <= xEnd; i++)
            {
                // North
                if (_World.tiles[i][yStart] != AIR)
                {
                    y = (float)yStart + 1 + EPSILON;
                    break;
                }
                // East
                else if (_World.tiles[i][yEnd] != AIR)
                {
                    y = (float)yEnd - size - EPSILON;
                    break;
                }
            }
        }
    }

    xStart = (int)SDL_floorf(x);
    yStart = (int)SDL_floorf(y);
    xEnd = (int)SDL_floorf(x + size);
    yEnd = (int)SDL_floorf(y + size);
    
    _Player.Velocity = {0, 0};
}