#ifndef ENTITY_H
#define ENTITY_H
#include "../Types.h"
#include <SDL2/SDL.h>

struct Entity
{
    static constexpr float EPSILON = 0.0001f;

    Vec2F Position;
    Vec2F Dimensions;
    Vec2F Center;

    Vec2F Velocity;
    float MaxVelocity;

    Vec2F Acceleration;
    float DragCoefficient;

    bool Elastic = false;

    EntityType Type;
    Uint8 CollisionType;
    Uint8 Collides;

    int xStart, xEnd;
    int yStart, yEnd;

    Entity(float x, float y, float w, float h);
    ~Entity();

    bool CanCollide(Entity *e)
    {
        return (CollisionType & e->Collides) && (e->CollisionType & Collides);
    }

    SDL_FRect ToFRect() const { return {Position.x, Position.y, Dimensions.x, Dimensions.y}; }
};

#endif
