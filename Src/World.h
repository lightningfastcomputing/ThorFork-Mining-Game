#ifndef WORLD_H
#define WORLD_H
#include <cstring>
#include <cstdlib>
#include <queue>
#include <time.h>
#include <functional>
#include <stdexcept>
#include "TileState.h"
#include "Player.h"
#include "ValueNoise2D.h"

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
    TileState TileStates[TILETYPE_COUNT];
    std::priority_queue<WorldAction, std::vector<WorldAction>, std::greater<WorldAction>> WorldActionQueue;
    Uint64 TickCount = 0;

    void GenerateVein(int x, int y, int count, TileType tileType);

    //set an impassable border around the world
    void SetBorder();

public:
    TileState **tiles;
    int Width;
    int Height;

    World(int width, int height);

    ~World();

    void Update(Uint64 tickCount);

    void ChangeTile(int x, int y, TileType tileType);

    void MineTile(int x, int y, int strength, Player& player);

    bool IsInBounds(int x, int y) const
    {
        return (x >= 0 && x < Width && y >= 0 && y < Height);
    };

    bool IsInBounds(const Vec2& point) const {
        return (point.x >= 0 && point.x < Width && point.y >= 0 && point.y < Height);
    };
};
#endif