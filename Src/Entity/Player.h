#ifndef PLAYER_H
#define PLAYER_H
#include "Entity.h"
#include <SDL2/SDL.h>

class Player : public Entity
{
private:

public:

    float Speed;

    direction Direction;
    
    int Score;
    Vec2F Target;
    bool CanMine;

    float MiningRadius;
    float DiscoverRadius;

    Entity* SelectedEntity = nullptr;

    Player(float width, float height, float speed);
    ~Player();
    void UpdateAcceleration(direction dir);
};

#endif
