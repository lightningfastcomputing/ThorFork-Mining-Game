#include "Chunk.h"

Chunk::Chunk(float x, float y, float w, float h, TileType mat) : Entity(x, y, w, h), Material(mat)
{
    Type = CHUNK;
    CollisionType = 1 << CHUNK;
    DragCoefficient = 0.15f;
    //Collides = 1 << CHUNK | 1 << PLAYER | 1 << DYNAMITE | 1 << MINECART;
}
Chunk::~Chunk()
{
}

std::string Chunk::DebugInfo()
{
    return std::format("ROCK CHUNK\n");
}