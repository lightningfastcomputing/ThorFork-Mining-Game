#include "EntityManager.h"

EntityManager::EntityManager(World &world, std::vector<Entity *> &entities) : _World(world), _Entities(entities)
{
    for (Entity *p : _Entities)
    {

        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;

        do
        {
            x = rand() % (_World.Width - 1) + 1;
            y = rand() % (_World.Height - 1) + 1;
            xStart = (int)SDL_floorf(x);
            xEnd = (int)SDL_floorf(x + w);
            yStart = (int)SDL_floorf(y);
            yEnd = (int)SDL_floorf(y + h);

        } while (!_World.IsInBounds(p->xStart, p->yStart) || !_World.IsInBounds(p->xEnd, p->yEnd));

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
    for (Entity *e : _Entities)
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

    for (Entity *p : _Entities)
    {
        // if the entity has a parent, skip it, it will be updated with the parent
        if (p->HasParent())
            continue;

        Vec2F &velocity = p->Velocity;

        p->Velocity += p->Acceleration;
        if (p->Velocity.Magnitude() > p->MaxVelocity)
        {
            p->Velocity.Normalize();
            p->Velocity = p->Velocity * p->MaxVelocity;
        }

        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;
        Vec2F positionDelta = {x, y};
        Entity *child = p->Child;
        float EPSILON = p->EPSILON;

        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (velocity.x != 0)
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                int xIdx = velocity.x < 0 ? xStart : xEnd;
                float xSnap = velocity.x < 0 ? (float)xStart + 1 + EPSILON : (float)xEnd - w - EPSILON;

                if (!_World.tiles[xIdx][i].Passable)
                {
                    x = xSnap;
                    if (!p->Elastic)
                        velocity.x = 0;
                    else
                        velocity.x = -velocity.x * 0.5f;
                    break;
                }
            }
            if (p->Corporeal)
            {
                for (Entity *other : _Entities)
                {
                    if (!other->Corporeal || other == p)
                        continue;

                    float xSnap = velocity.x < 0 ? other->BoundingBox.x + other->BoundingBox.w + EPSILON : other->BoundingBox.x - w - EPSILON;

                    if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                        x = xSnap;
                }
            }
        }

        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (velocity.y != 0)
        {
            int yIdx = velocity.y < 0 ? yStart : yEnd;
            float ySnap = velocity.y < 0 ? (float)yStart + 1 + EPSILON : (float)yEnd - h - EPSILON;

            for (int i = xStart; i <= xEnd; i++)
            {
                if (!_World.tiles[i][yIdx].Passable)
                {
                    y = ySnap;
                    if (!p->Elastic)
                        velocity.y = 0;
                    else
                        velocity.y = -velocity.y * 0.5f;
                    break;
                }
            }
            if (p->Corporeal)
            {
                for (Entity *other : _Entities)
                {
                    if (!other->Corporeal || other == p)
                        continue;

                    float ySnap = velocity.y < 0 ? other->BoundingBox.y + other->BoundingBox.h + EPSILON : other->BoundingBox.y - h - EPSILON;

                    if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                    {
                        y = ySnap;
                    }
                }
            }
        }

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        p->Center = {p->BoundingBox.x + p->HalfDimensions.x, p->BoundingBox.y + p->HalfDimensions.y};
        p->Velocity = p->Velocity * p->DragCoefficient; // friction

        if (child != nullptr)
        {
            float &x = child->BoundingBox.x, &y = child->BoundingBox.y;
            Vec2F &center = child->Center;

            x = p->Center.x + child->ParentOffset.x - child->HalfDimensions.x;
            y = p->Center.y + child->ParentOffset.y - child->HalfDimensions.x;
            center = {x + child->HalfDimensions.x, y + child->HalfDimensions.y};

            child->xStart = (int)SDL_floorf(x);
            child->yStart = (int)SDL_floorf(y);
            child->xEnd = (int)SDL_floorf(x + child->BoundingBox.w);
            child->yEnd = (int)SDL_floorf(y + child->BoundingBox.h);
        }

        if (p->Velocity.Magnitude() < 0.001f)
            p->Velocity = {0, 0};

        positionDelta.x = x - positionDelta.x;
        positionDelta.y = y - positionDelta.y;

        if (p->type == EntityType::PLAYER)
        {
            Player *player = static_cast<Player *>(p);
            player->Target.x += positionDelta.x;
            player->Target.y += positionDelta.y;
        }
    }
}

Explosive *EntityManager::SpawnExplosive(float x, float y)
{
    Explosive *e = new Explosive(x, y);
    _Entities.emplace_back(e);
    return e;
}