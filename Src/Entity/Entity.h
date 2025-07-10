#ifndef ENTITY_H
#define ENTITY_H
#include "../Types.h"
#include <SDL2/SDL.h>

class Entity
{
private:
    /* data */
public:
    static constexpr float EPSILON = 0.0001f;

    SDL_FRect BoundingBox;
    Vec2F HalfDimensions;
    Vec2F Center;

    Vec2F Velocity;

    EntityType type;

    int xStart, xEnd;
    int yStart, yEnd;

    Entity(float x, float y, float w, float h);
    ~Entity();
};


#endif
