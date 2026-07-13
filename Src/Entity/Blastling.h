#ifndef BLASTLING_H
#define BLASTLING_H

#include "Entity.h"

class Blastling : public Entity
{
public:
    float ExplosionRadius;

    Blastling(float x, float y);
    ~Blastling();

    std::string DebugInfo() override;
};

#endif
