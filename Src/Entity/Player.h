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
    static constexpr int MaxHealth = 3;
    int Health;
    Vec2F Target;
    bool CanMine;

    float MiningRadius;
    float DiscoverRadius;
    Entity* SelectedEntity = nullptr;
    Entity *EquippedEntity = nullptr;

    bool InteractMode;

    Player(float width, float height, float speed);
    ~Player();

    void Interact();
    void TakeDamage(int amount);
    void UpdateAcceleration(direction dir);

    std::string DebugInfo();
    
};

#endif
