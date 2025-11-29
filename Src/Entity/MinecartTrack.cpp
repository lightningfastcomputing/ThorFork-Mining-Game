#include "MinecartTrack.h"

MinecartTrack::MinecartTrack(float x, float y) : Entity(x, y, 1.0f, 1.0f)
{
    Static = true;
    Type = MINECART_TRACK;
    CollisionType = 1 << MINECART_TRACK;
    Collides = 0;
}
MinecartTrack::~MinecartTrack()
{
}

std::string MinecartTrack::DebugInfo()
{
    return "MINECART TRACK\n";
}