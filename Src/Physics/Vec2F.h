#ifndef VEC2F_H
#define VEC2F_H

#include "Vec2.h"

namespace Vec2F
{

    struct Vec2F
    {
        float x, y;
        Vec2F();
        Vec2F(float x, float y);
        // Vec2F(Vec2 vec);
        ~Vec2F();
        Vec2F operator+(Vec2F const &other);
    };

    Vec2F::Vec2F()
    {
    }

    Vec2F::Vec2F(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    // Vec2F::Vec2F(Vec2 vec)
    // {
    //     this->x = static_cast<float>(vec.x);
    //     this->y = static_cast<float>(vec.y);
    // }

    Vec2F::~Vec2F()
    {
    }

    Vec2F Vec2F::operator+(Vec2F const &other)
    {
        float resX = x + other.x;
        float resY = y + other.y;
        Vec2F res(resX, resY);
        return res;
    }
}

#endif
