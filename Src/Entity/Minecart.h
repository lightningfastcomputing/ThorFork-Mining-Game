#ifndef MINECART_H
#define MINECART_H

#include "Entity.h"
#include <vector>
#include <functional>
#include <ranges>
#include <memory>
#include <climits>
#include <algorithm>
#include <cstdio>


struct Minecart : public Entity
{
    float Capacity;
    float Weight;
    bool OnTrack;
    Minecart(float x, float y);
    Minecart(float x, float y, float w, float h, float capacity);
    ~Minecart();
    bool AddEntity(Entity* e);
    bool RemoveEntity(Entity *e);

    std::string DebugInfo();
    
};

#endif