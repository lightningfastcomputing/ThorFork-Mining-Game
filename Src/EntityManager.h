#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "World.h"
#include "Player.h"
#include <SDL2/SDL.h>

class EntityManager
{
private:
    World &_World;
    Player &_Player;
    void PlayerTryMine();
    void PlayerRadialDiscover();
    void UpdatePlayerPosition();

public:
    EntityManager(World &world, Player &player);
    ~EntityManager();
    void Update();
};
#endif
