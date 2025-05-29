#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "World.h"
#include "Player.h"
#include "Utils.h"
#include <SDL2/SDL.h>

class EntityManager
{
private:
    World &_World;
    std::vector<Player*> &_Players;
    void PlayerTryMine();
    void PlayerRadialDiscover(Uint64 deltaTime);
    void UpdatePlayerPosition();
    void EntityWorldCollisionCheck();
    void EntityEntityCollisionCheck();

public:
    EntityManager(World &world, std::vector<Player*> &players);
    ~EntityManager();
    void Update(Uint64 deltaTime);
};
#endif
