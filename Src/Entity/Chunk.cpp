#include "Chunk.h"

Chunk::Chunk(float x, float y, float w, float h, TileType mat) : Entity(x, y, w, h), Material(mat)
{
    Corporeal = true;
    type = CHUNK; 
    DragCoefficient = 0.75f;
}
Chunk::~Chunk()
{
}