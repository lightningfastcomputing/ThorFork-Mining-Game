#include "Entity.h"

Entity::Entity(float x, float y, float w, float h)
{
    //keep in this order so we can cast to an SDL_FRect
    Position = {x - w/2, y - h/2};
    Dimensions = {w, h};

    Center = {x, y};
    
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

void Entity::ReleaseChild(Vec2F offset)
{
    //fix
    Child->Parent = nullptr;
    Child->ParentOffset = {0, 0};
    Child = nullptr;
}
