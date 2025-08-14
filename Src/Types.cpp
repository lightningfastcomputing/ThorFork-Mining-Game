#include "Types.h"

Vec2F Vec2::ToVec2F() const
{
    return {(float)x, (float)y};
}

Vec2 Vec2::operator+(const Vec2 &other) const
{
    return {x + other.x, y + other.y};
}

Vec2 Vec2::operator-(const Vec2 &other) const
{
    return {x - other.x, y - other.y};
}

Vec2 Vec2::operator*(int scalar) const
{
    return {x * scalar, y * scalar};
}

Vec2 Vec2::operator/(int scalar) const
{
    return {x / scalar, y / scalar};
}

void Vec2::operator+=(const Vec2 &other)
{
    x += other.x;
    y += other.y;
}

float Vec2F::Magnitude() const
{
    return (SDL_sqrtf((x * x) + (y * y)));
}

void Vec2F::Normalize()
{
    float magnitude = this->Magnitude();
    if (magnitude != 0)
    {
        float invMagnitude = 1 / (magnitude);
        x *= invMagnitude;
        y *= invMagnitude;
    }
}

Vec2 Vec2F::ToVec2() const
{
    return {(int)x, (int)y};
}

Vec2F Vec2F::Floor() const
{
    return {SDL_floorf(x), SDL_floorf(y)};
}


Vec2F Vec2F::operator+(const Vec2F &other) const
{
    return {x + other.x, y + other.y};
}

Vec2F Vec2F::operator-(const Vec2F &other) const
{
    return {x - other.x, y - other.y};
}

Vec2F Vec2F::operator*(float scalar) const
{
    return {x * scalar, y * scalar};
}
Vec2F Vec2F::operator/(float scalar) const
{
    return {x / scalar, y / scalar};
}

void Vec2F::operator+=(const Vec2F &other)
{
    x += other.x;
    y += other.y;
}