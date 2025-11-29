#ifndef MINECARTTRACK_H
#define MINECARTTRACK_H

#include "Entity.h"

struct MinecartTrack : public Entity
{
    MinecartTrack(float x, float y);
    ~MinecartTrack();

    std::string DebugInfo();
};

#endif