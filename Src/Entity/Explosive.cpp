#include "Explosive.h"

Explosive::Explosive(float x, float y, float size, float explRadius) : Entity(x, y, size, size)
{
    ExplosionRadius = explRadius;
    Corporeal = true;
    type = DYNAMITE;
}

Explosive::~Explosive()
{

}