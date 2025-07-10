#include "EntityManager.h"

EntityManager::EntityManager(World &world, std::vector<Entity*>& entities) : _World(world), _Entities(entities)
{
    for (Entity* p : _Entities)
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
    for (Entity* e : _Entities)
    {
        delete e;
    }
}

void EntityManager::Update(Uint64 deltaTime)
{
    UpdatePlayerPosition();
}

void EntityManager::UpdatePlayerPosition()
{ 

    for (Entity* p : _Entities)
    {
        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        Vec2F velocity = p->Velocity;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;
        Vec2F positionDelta = {x, y};
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

            for (Entity* other : _Entities)
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

            for (Entity* other : _Entities)
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

            for (Entity* other : _Entities)
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

            for (Entity* other : _Entities)
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

        positionDelta.x = x - positionDelta.x;
        positionDelta.y = y - positionDelta.y;

        if (p->type == EntityType::PLAYER)
        {
            Player* player = static_cast<Player*>(p);
            player->Target.x += positionDelta.x;
            player->Target.y += positionDelta.y;
        }
    }
}

void EntityManager::SpawnExplosive(float x, float y)
{
    Explosive* e = new Explosive(x,y);
    _Entities.emplace_back(e);
}