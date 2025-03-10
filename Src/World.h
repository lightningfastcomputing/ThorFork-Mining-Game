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

struct WorldAction
{
    std::function<void()> Action = [] {};
    Uint64 TickExecuted;

    bool operator>(const WorldAction &other) const
    {
        return TickExecuted > other.TickExecuted; // Smallest TickExecuted first
    }
};

class World
{
private:
    TileState TileStates[TILETYPE_COUNT];
    std::priority_queue<WorldAction, std::vector<WorldAction>, std::greater<>> WorldActionQueue;
    Uint64 TicksPassed = 0;

    //"sprinkle" a tile around the map
    void Sprinkle(int count, TileState tileState, bool overwrite, int indexes[]);

    // encapsulate a specific tile with a material
    void Encapsulate(int count, TileState tileState, int index);

public:
    TileState **tiles;
    int Width;
    int Height;

    World(int width, int height, int nuggetCount, int stoneThickness, int explosiveCount);

    ~World();

    void Update();

    void ChangeTile(int x, int y, const TileState &tileState);

    void MineTile(int x, int y, Player& player);

    bool IsInBounds(int x, int y) const
    {
        return (x >= 0 && x < Width && y >= 0 && y < Height);
    };

    bool IsInBounds(const Vec2& point) const {
        return (point.x >= 0 && point.x < Width && point.y >= 0 && point.y < Height);
    };
};
#endif