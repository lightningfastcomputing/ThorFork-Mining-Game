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
    float MaxVelocity;

    Vec2F Acceleration;
    float DragCoefficient;

    bool Elastic = false;
    bool Corporeal;

    EntityType type;

    int xStart, xEnd;
    int yStart, yEnd;

    Entity *Parent = nullptr;
    Entity *Child = nullptr;

    Vec2F ParentOffset = {0, 0};

    Entity(float x, float y, float w, float h);
    ~Entity();
    bool HasParent() { return Parent != nullptr; }
    bool SetChild(Entity* child, Vec2F offset);
    void ReleaseChild();
};

#endif
