#include "Explosive.h"

Explosive::Explosive(float x, float y) : Entity(x, y, 0.9f, 0.9f)
{
    ExplosionRadius = 3;

    Type = DYNAMITE;
    CollisionType = 1 << DYNAMITE;
    //Collides = 1 << DYNAMITE | 1 << CHUNK | 1 << MINECART | 1 << PLAYER;
}

Explosive::Explosive(float x, float y, float size, float explRadius) : Entity(x, y, size, size)
{
    ExplosionRadius = explRadius;

    Type = DYNAMITE;
    CollisionType = 1 << DYNAMITE;
    //Collides = 1 << DYNAMITE | 1 << CHUNK | 1 << MINECART | 1 << PLAYER;
}

std::string Explosive::DebugInfo()
{
    return std::format("DYNAMITE\n ExplosionRadius: {}\n\n", ExplosionRadius);
}

Explosive::~Explosive()
{
}