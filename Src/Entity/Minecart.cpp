#include "Minecart.h"

Minecart::Minecart(float x, float y) : Entity(x, y, 1.98f, 3.98f)
{
    Capacity = 20;
    OnTrack = false;
    Type = MINECART;
    CollisionType = 1 << MINECART;
    // Collides = 1 << MINECART | 1 << CHUNK | 1 << PLAYER | 1 << DYNAMITE;
}
Minecart::Minecart(float x, float y, float w, float h, float capacity) : Entity(x, y, w, h), Capacity(capacity)
{
    Type = MINECART;
    OnTrack = false;
    CollisionType = 1 << MINECART;
    // Collides = 1 << MINECART | 1 << CHUNK | 1 << PLAYER | 1 << DYNAMITE;
}
Minecart::~Minecart()
{
    for (Entity *e : Children)
    {
        e->Parent = nullptr;
    }
}

bool Minecart::AddEntity(Entity *e)
{
    if (e->Type == PLAYER && !OnTrack)
    {
        return false;
    }
    if (Children.size() >= Capacity)
    {
        e->Acceleration = (e->Center - Center) * 10;
        return false;
    }

    e->Collides = 0;
    e->Parent = this;

    Children.push_back(e);

    Vec2F minPos = Position + Vec2F(0.1f, 0.1f);
    Vec2F maxPos = Position + Dimensions - Vec2F(0.1f, 0.1f) - e->Dimensions;

    e->Position = {Utils::RandFloat(minPos.x, maxPos.x), Utils::RandFloat(minPos.y, maxPos.y)};
    e->UpdateTileBounds();
    return true;
}

bool Minecart::RemoveEntity(Entity *e)
{
    if (!e)
        return false;

    e->Parent = nullptr;
    Children.erase(std::remove(Children.begin(), Children.end(), e), Children.end());
    e->Collides = UINT8_MAX;
    return true;
}

std::string Minecart::DebugInfo()
{
    return std::format("MINECART\n ITEMS: {}/{}\n", Children.size(), Capacity);
}

