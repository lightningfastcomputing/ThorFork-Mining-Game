#include "EntityManager.h"

EntityManager::EntityManager(World &world, std::vector<Player*>& players) : _World(world), _Players(players)
{
    for (Player* p : _Players)
    {
        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;

        do {
            x = rand() %  (_World.Width - 1) + 1;
            y = rand() %  (_World.Height - 1) + 1;
            xStart = (int)SDL_floorf(x);
            xEnd = (int)SDL_floorf(x + w);
            yStart = (int)SDL_floorf(y);
            yEnd = (int)SDL_floorf(y + h);

        }
        while (!_World.IsInBounds(p->xStart, p->yStart) || !_World.IsInBounds(p->xEnd, p->yEnd));

        for (int i = xStart; i <= xEnd; i++)
        {
            for (int j = yStart; j <= yEnd; j++)
            {
                _World.ChangeTile(i, j, TileType::AIR);
            }
        }
        p->Center = {x + p->HalfDimensions.x, y + p->HalfDimensions.y};

        
    }
}

EntityManager::~EntityManager()
{
}

void EntityManager::Update(Uint64 deltaTime)
{
    UpdatePlayerPosition();
    //PlayerRadialDiscover(deltaTime);
    //PlayerTryMine();
}


// space and computation tradeoff: use more rays to parallelize the computation at the cost of space
// or do every 2 frames, or every 3, whatever keeps the illusion

void EntityManager::UpdatePlayerPosition()
{ 

    for (Player* p : _Players)
    {
        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        Vec2F velocity = p->Velocity;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;
        float EPSILON = p->EPSILON;

        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        // West Collision
        if (velocity.x < 0)
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                
                if (!_World.tiles[xStart][i].Passable)
                {
                    x = (float)xStart + 1 + EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                    x = other->BoundingBox.x + other->BoundingBox.w + EPSILON;
            }
        }
        // East Collision
        else if (velocity.x > 0)
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                
                if (!_World.tiles[xEnd][i].Passable)
                {
                    x = (float)xEnd - w - EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                    x = other->BoundingBox.x - w - EPSILON;
            }
        }
        
        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        // North Collision
        if (velocity.y < 0) {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (!_World.tiles[i][yStart].Passable)
                {
                    y = (float)yStart + 1 + EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                {
                    y = other->BoundingBox.y + other->BoundingBox.h + EPSILON;
                }
            }
            
        }
        // South Collision
        else if (velocity.y > 0)
        {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (!_World.tiles[i][yEnd].Passable)
                {
                    y = (float)yEnd - h - EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                {
                    y = other->BoundingBox.y - h - EPSILON;
                }
            }
        }

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        p->Center = {p->BoundingBox.x + p->HalfDimensions.x, p->BoundingBox.y + p->HalfDimensions.y};
        p->Velocity = {0, 0};
    }
}

void EntityManager::FixPlayerTargets() 
{
    for (Player *p : _Players)
    {
        Vec2F targetVector = p->Target - p->Center;
        float mag = targetVector.Magnitude();
        if (mag > p->MiningRadius)
        {
            float invMag = 1/mag;

            p->Target = (targetVector * invMag) * p->MiningRadius;
        }
    }
}

void EntityManager::EntityWorldCollisionCheck()
{

}

void EntityManager::EntityEntityCollisionCheck()
{

}