#include "EntityManager.h"

EntityManager::EntityManager(World &world, Player &player) : _World(world), _Player(player)
{
    _Player.BoundingBox.x = _World.Width / 2;
    _Player.BoundingBox.y = _World.Height / 2;
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
    float &x = _Player.BoundingBox.x, &y = _Player.BoundingBox.y, w = _Player.BoundingBox.w, h = _Player.BoundingBox.h;
    float EPSILON = _Player.EPSILON;

    if (velocity.x != 0)
    {
        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (xStart < 0)
        {
            x = EPSILON;
        }
        else if (xEnd > _World.Width - 1)
        {
            x = _World.Width - w - EPSILON;
        }
        else
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                // West Collision
                if (_World.tiles[xStart][i] != AIR)
                {
                    x = (float)xStart + 1 + EPSILON;
                    break;
                }
                // East Collision
                else if (_World.tiles[xEnd][i] != AIR)
                {
                    x = (float)xEnd - w - EPSILON;
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
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (yStart < 0)
        {
            y = EPSILON;
        }
        else if (yEnd > _World.Height - 1)
        {
            y = _World.Height - h - EPSILON;
        }
        else
        {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (_World.tiles[i][yStart] != AIR)
                {
                    y = (float)yStart + 1 + EPSILON;
                    break;
                }
                else if (_World.tiles[i][yEnd] != AIR)
                {
                    y = (float)yEnd - h - EPSILON;
                    break;
                }
            }
        }
    }

    xStart = (int)SDL_floorf(x);
    yStart = (int)SDL_floorf(y);
    xEnd = (int)SDL_floorf(x + w);
    yEnd = (int)SDL_floorf(y + h);

    _Player.Velocity = {0, 0};
}