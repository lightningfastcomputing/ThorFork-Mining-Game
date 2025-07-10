#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "World.h"
#include "Entity/Player.h"
#include "Entity/Explosive.h"
#include "Utils.h"
#include <memory>
#include <SDL2/SDL.h>

class EntityManager
{
private:
    World &_World;
    std::vector<Entity*> &_Entities;
    void UpdatePlayerPosition();

public:
    EntityManager(World &world, std::vector<Entity*> &entities);
    ~EntityManager();
    void Update(Uint64 deltaTime);
    void SpawnExplosive(float x, float y);
};
#endif
