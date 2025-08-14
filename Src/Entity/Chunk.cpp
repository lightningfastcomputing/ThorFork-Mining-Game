#include "Chunk.h"

Chunk::Chunk(float x, float y, float w, float h, TileType mat) : Entity(x, y, w, h), Material(mat)
{
    DragCoefficient = 0.75f;

    Type = CHUNK;
    CollisionType = 1 << CHUNK;
    Collides = 1 << CHUNK | 1 << PLAYER | 1 << DYNAMITE;
}
Chunk::~Chunk()
{
}