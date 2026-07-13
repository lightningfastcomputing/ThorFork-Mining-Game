#ifndef WORLD_H
#define WORLD_H

#include <unordered_set>

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
#include "Entity/Player.h"
#include "Entity/Explosive.h"
#include "Entity/Chunk.h"
#include "Entity/Minecart.h"
#include "Entity/MinecartTrack.h"
#include "Entity/Blastling.h"
#include "Utils.h"
#include "Types.h"
#include <memory>
#include <ranges>
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
    Player *_Player = nullptr;
    Uint64 NextBlastlingSpawn = 0;
    static constexpr int MaxBlastlings = 8;

    int KillDepth = 0;
    std::vector<Entity *> PendingDeletes;
    std::unordered_set<Entity *> PendingDeleteSet;

    void FlushPendingDeletes();

    void InitTiles();
    void InitEntities();
    void SetBorder();
    void UpdateEntities();
    void TrySpawnBlastlingAtDiscoveryEdge();

    void Explosion(Explosive *e);
    void BlastAt(Vec2F pos, float radius);
    std::vector<Vec2> GetTiles(Vec2F pos, float radius);
    std::vector<Entity*> GetEntities(Vec2F pos, float radius);

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
    Blastling *SpawnBlastling(float x, float y);
    Chunk *SpawnChunk(Vec2F pos, Vec2F dim, TileType type);

    Entity* SpawnEntity(Vec2F pos, EntityType type);

    void KillEntity(Entity *&entity);
    Entity *FindEntity(Vec2F pos, int type) const;

    void MineTile(int x, int y, int strength, bool audible);

    void Update(Uint64 tickCount);
};

#endif
