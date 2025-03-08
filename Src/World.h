#ifndef WORLD_H
#define WORLD_H
#include <cstring>
#include <cstdlib>
#include <queue>
#include <time.h>
#include <functional>
#include <stdexcept>
#include "Types.h"

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
    
    std::priority_queue<WorldAction, std::vector<WorldAction>, std::greater<>> WorldActionQueue;
    Uint64 TicksPassed = 0;

    //"sprinkle" a tile around the map
    void Sprinkle(int count, Tile tile, bool overwrite, int indexes[]);

    // encapsulate a specific tile with a material
    void Encapsulate(int count, Tile tile, int index);

public:
    Tile **tiles;
    int Width;
    int Height;

    World();

    World(int width, int height, int nuggetCount, int stoneThickness, int explosiveCount);

    ~World();

    void Update();

    void ChangeTile(int x, int y, Tile tile);

    void DestroyTile(int x, int y);

    bool IsInBounds(int x, int y) const
    {
        return (x >= 0 && x < Width && y >= 0 && y < Height);
    };

    bool IsInBounds(const Vec2& point) const {
        return (point.x >= 0 && point.x < Width && point.y >= 0 && point.y < Height);
    };
};
#endif