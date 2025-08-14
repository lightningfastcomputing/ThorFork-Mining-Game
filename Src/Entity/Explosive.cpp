#include "Explosive.h"

Explosive::Explosive(float x, float y, float size, float explRadius) : Entity(x, y, size, size)
{
    ExplosionRadius = explRadius;

    Type = DYNAMITE;
    CollisionType = 1 << DYNAMITE;
    Collides = 1 << DYNAMITE | 1 << CHUNK;
}

Explosive::~Explosive()
{

}