#include "EntityManager.h"

EntityManager::EntityManager(World &world, Player &player) : _World(world), _Player(player)
{
}

EntityManager::~EntityManager() {

}

void EntityManager::UpdatePlayerPosition()
{
    bool NW, NE, SW, SE;
    int xStart, yStart, xEnd, yEnd;

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

            NW = _World.tiles[xStart][yStart] != AIR;
            NE = _World.tiles[xEnd][yStart] != AIR;
            SW = _World.tiles[xStart][yEnd] != AIR;
            SE = _World.tiles[xEnd][yEnd] != AIR;

            if (NW || SW)
            {
                x = (float)xStart + 1 + EPSILON;
            }
            else if (NE || SE)
            {
                x = (float)xEnd - size - EPSILON;
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
            NW = _World.tiles[xStart][yStart] != AIR;
            NE = _World.tiles[xEnd][yStart] != AIR;
            SW = _World.tiles[xStart][yEnd] != AIR;
            SE = _World.tiles[xEnd][yEnd] != AIR;

            if (NW || NE)
            {
                y = (float)yStart + 1 + EPSILON;
            }
            else if (SW || SE)
            {
                y = (float)yEnd - size - EPSILON;
            }
        }
    }
    _Player.Velocity = {0, 0};
}