#include "World.h"

using namespace std;

World::World()
{
    Width = 20;
    Height = 20;
    this->tiles = new Tile *[Width];

    for (int i = 0; i < Width; i++)
    {
        this->tiles[i] = new Tile[Height];
        memset(tiles[i], AIR, Height);
    }
};

World::World(int width, int height, int nuggetCount, int stoneThickness, int explosiveCount)
{
    this->Width = width;
    this->Height = height;
    this->tiles = new Tile *[Width];

    for (int i = 0; i < Width; i++)
    {
        this->tiles[i] = new Tile[Height];
        for (int j = 0; j < Height; j++)
            this->tiles[i][j] = AIR;
    }

    int *nuggets = new int[nuggetCount];
    Sprinkle(nuggetCount, GOLD, false, nuggets);
    Sprinkle(nuggetCount, GOLD, false, nullptr);
    for (int i = 0; i < nuggetCount; i++)
        this->Encapsulate(stoneThickness, STONE, nuggets[i]);
    delete[] nuggets;

    Sprinkle(explosiveCount, EXPLOSIVE, true, NULL);
}
World::~World()
{
    for (int i = 0; i < Width; i++)
    {
        delete[] tiles[i];
    }
    delete[] tiles;
}

void World::ChangeTile(int x, int y, Tile tile)
{
    if (x > 0 && y > 0 && x < Width && y < Height)
        this->tiles[x][y] = tile;
}

void World::Sprinkle(int count, Tile tile, bool overwrite, int indexes[])
{
    int i = 0;
    while (i < count)
    {
        int randX = rand() % Width, randY = rand() % Height;
        if (this->tiles[randX][randY] == AIR || overwrite)
        {
            this->tiles[randX][randY] = tile;
            if (indexes)
                indexes[i] = randX + randY * Width;
            i++;
        }
        else
            continue;
    }
}

void World::Encapsulate(int count, Tile tile, int index)
{
    if (count == 0)
        return;

    int adjacents[4];
    adjacents[0] = index - 1;
    adjacents[1] = index + 1;
    adjacents[2] = index - Width;
    adjacents[3] = index + Width;

    for (int i = 0; i < 4; i++)
    {
        int x = adjacents[i] % Width, y = adjacents[i] / Width;
        if (IsInBounds(x, y) && this->tiles[x][y] == AIR)
        {
            this->tiles[x][y] = tile;
        }
    }

    this->Encapsulate(count - 1, tile, adjacents[rand() % 4]);
}

int World::DestroyTile(int x, int y)
{
    if (x < 0 || y < 0 || x >= Width || y >= Height)
        return 0;
    int score = 0;
    int index = x + y * Width;
    switch (tiles[x][y])
    {
    case AIR:
        break;
    case STONE:
        this->tiles[x][y] = AIR;
        break;
    case EXPLOSIVE: //obsolete, needs to be fixed
        int adjacents[8];
        adjacents[0] = index - 1;
        adjacents[1] = index + 1;
        adjacents[2] = index - Width;
        adjacents[3] = index + Width;
        adjacents[4] = index - Width - 1;
        adjacents[5] = index - Width + 1;
        adjacents[6] = index + Width - 1;
        adjacents[7] = index + Width + 1;

        tiles[x][y] = AIR;
        for (int i = 0; i < 8; i++)
        {
            if (adjacents[i] < Width * Height && adjacents[i] > 0)
                score += DestroyTile(adjacents[i] % Width, adjacents[i] / Width);
        }
        break;
    case GOLD:
        score = 1;
        tiles[x][y] = AIR;
        break;
    }
    return score;
}

bool World::IsInBounds(int x, int y) {
    return (x >= 0 && x < Width && y >= 0 && y < Height);
}