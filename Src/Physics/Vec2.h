#ifndef VEC2_H
#define VEC2_H

#include "Vec2F.h"

struct Vec2
{
    int x, y;
    Vec2();
    Vec2(int x, int y);
    Vec2(Vec2F::Vec2F &fVect);
    ~Vec2();
    Vec2 operator+(Vec2 const &other);
};

Vec2::Vec2()
{
}

Vec2::Vec2(int x, int y)
{
    this->x = x;
    this->y = y;
}

Vec2::Vec2(Vec2F::Vec2F &fVect)
{
    this->x = (int)(fVect.x);
    this->y = (int)(fVect.y);
}

Vec2::~Vec2()
{
}

inline Vec2 Vec2::operator+(Vec2 const &other)
{
    float resX = x + other.x;
    float resY = y + other.y;
    Vec2 res(resX, resY);
    return res;
}

#endif