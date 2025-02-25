#ifndef UTILS_H
#define UTILS_H
#include <SDL2/SDL.h>


struct Utils
{
    static // dst = sqrt((x-x0)^2 + (y-y0^2))
        float
        Distance(float x0, float y0, float x1, float y1)
    {
        float distance = SDL_sqrtf(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0)));
        // printf("DISTANCE: %f\n", distance);
        return distance;
    }
};

#endif