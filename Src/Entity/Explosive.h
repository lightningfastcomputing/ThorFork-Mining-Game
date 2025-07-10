#ifndef EXPLOSIVE_H
#define EXPLOSIVE_H

#include "Entity.h"

struct Explosive : public Entity
{
    Explosive(float x, float y);
    ~Explosive();
};

#endif
