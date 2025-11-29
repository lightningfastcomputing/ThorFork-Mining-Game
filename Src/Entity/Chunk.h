#ifndef CHUNK_H
#define CHUNK_H

#include "Entity.h"

struct Chunk : public Entity
{
    TileType Material;
    Chunk(float x, float y, float w, float h, TileType mat);
    ~Chunk();

    std::string DebugInfo();
};

#endif