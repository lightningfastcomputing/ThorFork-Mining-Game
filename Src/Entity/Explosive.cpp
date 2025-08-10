#include "Explosive.h"

Explosive::Explosive(float x, float y) : Entity(x, y, 0.7f, 0.7f)
{
    Corporeal = false;
    type = DYNAMITE;
}

Explosive::~Explosive()
{

}