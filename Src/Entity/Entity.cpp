#include "Entity.h"

Entity::Entity(float x, float y, float w, float h)
{
    Position = {x - w / 2, y - h / 2};
    Dimensions = {w, h};

    Center = {x, y};

    Velocity = {0, 0};
    MaxVelocity = 0.05;
    Acceleration = {0, 0};
    DragCoefficient = 0.7;

    xStart = (int)SDL_floorf(x);
    xEnd = (int)SDL_floorf(x + w);
    yStart = (int)SDL_floorf(y);
    yEnd = (int)SDL_floorf(y + h);
}

Entity::~Entity()
{
}
