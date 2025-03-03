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
    srand(time(0));

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

void World::Update()
{
    while (!WorldActionQueue.empty() && WorldActionQueue.top().TickExecuted <= TicksPassed)
    {
        WorldAction worldAction = WorldActionQueue.top();
        WorldActionQueue.pop();

        worldAction.Action();
    }
    TicksPassed++;
}

void World::ChangeTile(int x, int y, Tile tile)
{
    this->tiles[x][y] = tile;
}

void World::DestroyTile(int x, int y)
{
    if (IsInBounds(x, y))
    {
        switch (tiles[x][y])
        {
        case AIR:
            return;
        case STONE:
        case GOLD:
            WorldActionQueue.push({[this, x, y]()
                                        { this->ChangeTile(x, y, AIR); }, TicksPassed});
            break;
        case EXPLOSIVE:
            Vec2 adjacents[4];
            adjacents[0] = {x - 1, y};
            adjacents[1] = {x + 1, y};
            adjacents[2] = {x, y - 1};
            adjacents[3] = {x, y + 1};

            WorldActionQueue.push({[this, x, y]()
                                        { this->ChangeTile(x, y, AIR); }, TicksPassed});
            for (int i = 0; i < 4; i++)
            {
                Vec2 point = adjacents[i];
                int x = point.x, y = point.y;

                if (IsInBounds(x, y) && tiles[x][y] != AIR)
                {
                    WorldActionQueue.push({[this, x, y]()
                                                { this->DestroyTile(x, y); }, TicksPassed + 5});
                }
            }
            break;
        default:
            throw std::runtime_error("Invalid tile\n");
            break;
        }
        // printf("Succeeded\n");
    }
}