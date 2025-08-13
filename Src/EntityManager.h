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
    std::vector<Entity *> &_Entities;
    SoundManager &_SoundManager;
    void UpdatePlayerPosition();

public:
    EntityManager(World &world, std::vector<Entity *> &entities, SoundManager &SoundManager);
    ~EntityManager();
    void Update(Uint64 deltaTime);
    void KillEntity(Entity *entity);
    Explosive *SpawnExplosive(float x, float y);
    Entity *FindEntity(Vec2F pos);
};
#endif
