#include "Blastling.h"

Blastling::Blastling(float x, float y) : Entity(x, y, 0.9f, 0.9f)
{
    Type = BLASTLING;
    CollisionType = 1 << BLASTLING;
    ExplosionRadius = 3.5f;
    MaxVelocity = 0.065f;
    DragCoefficient = 0.82f;
    Elastic = false;
    Killable = true;
}

Blastling::~Blastling()
{
}

std::string Blastling::DebugInfo()
{
    return "BLASTLING\n";
}
