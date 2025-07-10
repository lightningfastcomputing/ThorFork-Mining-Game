#include "Explosive.h"

Explosive::Explosive(float x, float y) : Entity(x, y, 2.9f, 2.9f)
{
    type = DYNAMITE;
}

Explosive::~Explosive()
{

}