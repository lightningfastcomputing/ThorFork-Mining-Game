#ifndef WORLD_H
#define WORLD_H

#include <cstdlib>
#include <vector>
#include <cstring>

enum tile
{
    AIR,
    STONE,
    GOLD,
    EXPLOSIVE,
};

class World
{
private:
    //"sprinkle" a tile around the map
    void Sprinkle(int count, tile tile, bool overwrite, int indexes[]);
    //encapsulate a specific tile with a material
    void Encapsulate(int count, tile tile, int index);

public:
    tile **tiles;
    int Width;
    int Height;

    World();

    World(int width, int height, int nuggetCount, int stoneThickness, int explosiveCount);

    ~World();

    void ChangeTile(int x, int y, tile tile);

    int DestroyTile(int x, int y);

    bool IsInBounds(int x, int y);
};
#endif