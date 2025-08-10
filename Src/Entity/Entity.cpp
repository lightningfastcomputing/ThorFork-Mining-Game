#include "Entity.h"

Entity::Entity(float x, float y, float w, float h)
{
    Center = {x, y};
    HalfDimensions = {w/2, h/2};
    BoundingBox = {Center.x - HalfDimensions.x, Center.y - HalfDimensions.y, w, h};
    Velocity = {0, 0};

    xStart = (int)SDL_floorf(x);
    xEnd = (int)SDL_floorf(x + w);
    yStart = (int)SDL_floorf(y);
    yEnd = (int)SDL_floorf(y + h);
}

Entity::~Entity()
{
}

bool Entity::SetChild(Entity* child, Vec2F offset)
{
    bool valid = child != nullptr;
    if (valid)
    {
        Child = child;
        child->Parent = this;
        child->ParentOffset = offset;
    }
    return valid;
}

void Entity::ReleaseChild()
{
    Child->Parent = nullptr;
    Child->ParentOffset = {0,0};
    Child = nullptr;
}
