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

    Vec2F Position;
    Vec2F Dimensions;
    Vec2F Center;

    Vec2F Velocity;
    float MaxVelocity;

    Vec2F Acceleration;
    float DragCoefficient;

    bool Elastic = false;
    bool Corporeal;

    EntityType type;

    int xStart, xEnd;
    int yStart, yEnd;

    Entity(float x, float y, float w, float h);
    ~Entity();

    SDL_FRect ToFRect() const { return {Position.x, Position.y, Dimensions.x, Dimensions.y}; }

};

#endif
