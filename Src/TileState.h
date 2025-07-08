#ifndef TILESTATE_H
#define TILESTATE_H

#include "Types.h"

struct TileState
{
    unsigned int TileType : 8;
    unsigned int Health : 8;
    unsigned int Passable : 1;
    unsigned int Indestrucible: 1;
};

#endif