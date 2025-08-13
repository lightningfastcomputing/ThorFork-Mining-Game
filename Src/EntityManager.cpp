#include "EntityManager.h"

EntityManager::EntityManager(World &world, std::vector<Entity *> &entities, SoundManager &soundManager) : _World(world), _Entities(entities), _SoundManager(soundManager)
{
    for (Entity *p : _Entities)
    {

        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        Vec2F &position = p->Position, dimensions = p->Dimensions * 2;

        do
        {

            position = {static_cast<float>(rand() % (_World.Width - 1) + 1), static_cast<float>(rand() % (_World.Height - 1) + 1)};

            xStart = (int)SDL_floorf(position.x);
            xEnd = (int)SDL_floorf(position.x + dimensions.x);
            yStart = (int)SDL_floorf(position.y);
            yEnd = (int)SDL_floorf(position.y + dimensions.y);

        } while (!_World.IsInBounds(p->xStart, p->yStart) || !_World.IsInBounds(p->xEnd, p->yEnd));

        for (int i = xStart; i <= xEnd; i++)
        {
            for (int j = yStart; j <= yEnd; j++)
            {
                _World.ChangeTile(i, j, TileType::AIR);
            }
        }
        p->Center = p->Position + p->Dimensions * 0.5f;
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

        Vec2F &velocity = p->Velocity;

        p->Velocity += p->Acceleration;
        if (p->Velocity.Magnitude() > p->MaxVelocity)
        {
            p->Velocity.Normalize();
            p->Velocity = p->Velocity * p->MaxVelocity;
        }

        if (p->Velocity.Magnitude() == 0)
        {
            continue;
        }

        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        float &x = p->Position.x, &y = p->Position.y, w = p->Dimensions.x, h = p->Dimensions.y;
        Vec2F positionDelta = {x, y};
        float EPSILON = p->EPSILON;

        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (velocity.x != 0)
        {
            float collisionVelocity = !p->Elastic ? 0 : -velocity.x * 0.5f;
            int xIdx = velocity.x < 0 ? xStart : xEnd;
            float xSnap = velocity.x < 0 ? (float)xStart + 1 + EPSILON : (float)xEnd - w - EPSILON;

            for (int i = yStart; i <= yEnd; i++)
            {

                if (!_World.tiles[xIdx][i].Passable)
                {
                    x = xSnap;
                    velocity.x = collisionVelocity;
                }
            }
            if (p->Corporeal)
            {
                for (Entity *other : _Entities)
                {
                    if (!other->Corporeal || other == p)
                        continue;

                    float xSnap = velocity.x < 0 ? other->Position.x + other->Dimensions.x + EPSILON
                                                 : other->Position.x - w - EPSILON;

                    SDL_FRect entityRect = p->ToFRect(), otherRect = other->ToFRect();
                    if (SDL_HasIntersectionF(&entityRect, &otherRect))
                    {
                        x = xSnap;
                        velocity.x = collisionVelocity;
                    }
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
            float collisionVelocity = !p->Elastic ? 0 : -velocity.y * 0.5f;
            int yIdx = velocity.y < 0 ? yStart : yEnd;
            float ySnap = velocity.y < 0 ? (float)yStart + 1 + EPSILON : (float)yEnd - h - EPSILON;

            for (int i = xStart; i <= xEnd; i++)
            {
                if (!_World.tiles[i][yIdx].Passable)
                {
                    y = ySnap;
                    velocity.y = collisionVelocity;
                }
            }
            if (p->Corporeal)
            {
                for (Entity *other : _Entities)
                {
                    if (!other->Corporeal || other == p)
                        continue;

                    float ySnap = velocity.y < 0 ? other->Position.y + other->Dimensions.y + EPSILON : other->Position.y - h - EPSILON;

                    SDL_FRect entityRect = p->ToFRect(), otherRect = other->ToFRect();
                    if (SDL_HasIntersectionF(&entityRect, &otherRect))
                    {
                        y = ySnap;
                        velocity.y = collisionVelocity;
                    }
                }
            }
        }

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        p->Center = p->Position + (p->Dimensions * 0.5f);
        p->Velocity = p->Velocity * p->DragCoefficient; // friction

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
    Explosive *e = new Explosive(x, y, 1.1f, 3.4f);
    _Entities.emplace_back(e);
    return e;
}

void EntityManager::KillEntity(Entity *entity)
{
    switch (entity->type)
    {
    case EntityType::DYNAMITE:
    {
        Explosive *e = static_cast<Explosive *>(entity);
        _World.Explosion(e->Center, e->ExplosionRadius);
        _SoundManager.PlaySound(Sound::EXPLOSION);
        _Entities.erase(std::remove(_Entities.begin(), _Entities.end(), e), _Entities.end());
        delete e;
    }
    break;

    case EntityType::PLAYER:
    case EntityType::ENTITYTYPE_COUNT:
        return;
    }
}

Entity *EntityManager::FindEntity(Vec2F pos)
{
    for (Entity* e : _Entities)
    {
        SDL_FPoint point = {pos.x, pos.y};
        SDL_FRect rect = e->ToFRect();
        if (SDL_PointInFRect(&point, &rect))
        {
            return e;
        }
    }
    return nullptr;
}
