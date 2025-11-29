#ifndef EXPLOSIVE_H
#define EXPLOSIVE_H

#include "Entity.h"

struct Explosive : public Entity
{
    float ExplosionRadius;
    Explosive(float x, float y);
    Explosive(float x, float y, float size, float explRadius);
    ~Explosive();

    std::string DebugInfo();
};

#endif
