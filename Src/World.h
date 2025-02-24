#ifndef WORLD_H
#define WORLD_H
#include <cstring>
#include <cstdlib>
#include "Types.h"

class World
{
private:
    //"sprinkle" a tile around the map
    void Sprinkle(int count, Tile tile, bool overwrite, int indexes[]);
    //encapsulate a specific tile with a material
    void Encapsulate(int count, Tile tile, int index);

public:
    Tile **tiles;
    int Width;
    int Height;

    World();

    World(int width, int height, int nuggetCount, int stoneThickness, int explosiveCount);

    ~World();

    void ChangeTile(int x, int y, Tile tile);

    int DestroyTile(int x, int y);

    bool IsInBounds(int x, int y);
};
#endif