#ifndef WORLD_H
#define WORLD_H

#include <cstring>
#include <cstdlib>
#include <queue>
#include <time.h>
#include <functional>
#include <climits>
#include <stdexcept>
#include "TileState.h"
#include "Entity/Player.h"
#include "ValueNoise2D.h"
#include "SoundManager.h"
#include "Entity/Explosive.h"
#include "Utils.h"
#include "Types.h"
#include <memory>
#include <SDL2/SDL.h>

struct WorldAction
{
    std::function<void()> Action = [] {};
    Uint64 TickExecuted;

    bool operator>(const WorldAction &other) const
    {
        return TickExecuted > other.TickExecuted;
    }
};

class World
{
private:
    std::priority_queue<WorldAction, std::vector<WorldAction>, std::greater<WorldAction>> WorldActionQueue;
    SoundManager &_SoundManager;
    Uint64 TickCount = 0;

    void InitTiles();
    void InitEntities();
    void SetBorder();
    void UpdateEntities();

    void Explosion(Explosive* e);

    void ChangeTile(int x, int y, TileType TileType);

public:
    int Width, Height;
    TileState **Tiles;
    std::vector<Entity *> _Entities;
    TileState TileStates[TILETYPE_COUNT];
    World(int Width, int Height, SoundManager &SoundManager);
    ~World();
    bool InBounds(Vec2 pos) const
    {
        return (pos.x >= 0 && pos.x < Width && pos.y >= 0 && pos.y < Height);
    }
    void AddPlayer(Player *player);
    Explosive *SpawnExplosive(float x, float y);
    void KillEntity(Entity *entity);
    Entity *FindEntity(Vec2F pos);

    void MineTile(int x, int y, int strength, Player *player);

    void Update(Uint64 tickCount);
};

#endif
