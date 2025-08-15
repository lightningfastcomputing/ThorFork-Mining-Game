#ifndef UTILS_H
#define UTILS_H
#include <SDL2/SDL.h>
#include "Types.h"

struct Utils
{
    static float Distance(Vec2F p0, Vec2F p1)
    {
        return SDL_sqrtf(((p1.x - p0.x) * (p1.x - p0.x)) + ((p1.y - p0.y) * (p1.y - p0.y)));
    }

    static bool IsEven(Uint64 value)
    {
        return (value & 1) == 0;
    }

    static inline int16_t clamp16(int sample)
    {
        if (sample > 32767)
            return 32767;
        if (sample < -32768)
            return -32768;
        return (int16_t)sample;
    }
};

#endif