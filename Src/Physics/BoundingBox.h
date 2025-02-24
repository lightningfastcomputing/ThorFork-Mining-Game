#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Vec2.h"
#include "Vec2F.h"

struct BoundingBox
{
    BoundingBox();
    BoundingBox(float x, float y, float size);
    ~BoundingBox();
    bool Collides(BoundingBox &other);
    bool Collides(int x, int y);
    bool Collides(Vec2 point);
    Vec2F::Vec2F Pos;
    float Size;
};

BoundingBox::BoundingBox()
{
}

BoundingBox::BoundingBox(float x, float y, float size)
{
    this->Pos = {x, y};
    this->Size = size;
}

BoundingBox::~BoundingBox()
{
}

bool BoundingBox::Collides(BoundingBox &other)
{
    return false; // implementation later
}

//
bool BoundingBox::Collides(int x, int y)
{

    return false; // implement later
}

bool BoundingBox::Collides(Vec2 point)
{
    return false; //implement later
}

#endif
