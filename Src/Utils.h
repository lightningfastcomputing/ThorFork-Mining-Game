#ifndef UTILS_H
#define UTILS_H
#include <SDL2/SDL.h>

struct Utils
{
    static float Distance(float x0, float y0, float x1, float y1)
    {
        float distance = SDL_sqrtf(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0)));
        // printf("DISTANCE: %f\n", distance);
        return distance;
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