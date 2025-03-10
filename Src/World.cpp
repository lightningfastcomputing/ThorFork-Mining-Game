#include "World.h"

using namespace std;

World::World(int width, int height, int nuggetCount, int stoneThickness, int explosiveCount)
{
    srand(time(0));

    TileStates[AIR] = {TileType::AIR, 0};
    TileStates[STONE] = {TileType::STONE, 10};
    TileStates[GOLD] = {TileType::GOLD, 5};
    TileStates[EXPLOSIVE] = {TileType::EXPLOSIVE, 1};

    this->Width = width;
    this->Height = height;
    this->tiles = new TileState *[Width];

    for (int i = 0; i < Width; i++)
    {
        this->tiles[i] = new TileState[Height];
        for (int j = 0; j < Height; j++)
            this->tiles[i][j] = TileStates[AIR];
    }

    int *nuggets = new int[nuggetCount];
    Sprinkle(nuggetCount, TileStates[GOLD], false, nuggets);
    Sprinkle(nuggetCount, TileStates[GOLD], false, nullptr);
    for (int i = 0; i < nuggetCount; i++)
        this->Encapsulate(stoneThickness, TileStates[STONE], nuggets[i]);
    delete[] nuggets;

    Sprinkle(explosiveCount, TileStates[EXPLOSIVE], true, NULL);
}
World::~World()
{
    for (int i = 0; i < Width; i++)
    {
        delete[] tiles[i];
    }
    delete[] tiles;
}

void World::Sprinkle(int count, TileState tileState, bool overwrite, int indexes[])
{
    int i = 0;
    while (i < count)
    {
        int randX = rand() % Width, randY = rand() % Height;
        if (this->tiles[randX][randY].TileType == TileType::AIR || overwrite)
        {
            this->tiles[randX][randY] = tileState;
            if (indexes)
                indexes[i] = randX + randY * Width;
            i++;
        }
        else
            continue;
    }
}

void World::Encapsulate(int count, TileState tileState, int index)
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
        if (IsInBounds(x, y) && this->tiles[x][y].TileType == TileType::AIR)
        {
            this->tiles[x][y] = tileState;
        }
    }

    this->Encapsulate(count - 1, tileState, adjacents[rand() % 4]);
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

void World::ChangeTile(int x, int y, const TileState &tileState)
{
    this->tiles[x][y] = tileState;
}

void World::MineTile(int x, int y, Player &player)
{
    if (IsInBounds(x, y))
    {
        TileState &tileState = tiles[x][y];

        tileState.Health--;
        if (tileState.Health <= 0)
        {

            switch (tileState.TileType)
            {
            case TileType::AIR:
                return;
            case TileType::STONE:
                WorldActionQueue.push({[this, x, y]()
                                       { this->ChangeTile(x, y, TileStates[AIR]); }, TicksPassed});
                break;
            case TileType::GOLD:
                WorldActionQueue.push({[this, x, y, &player]()
                                       {player.Score++; 
                                        this->ChangeTile(x, y, TileStates[AIR]); }, TicksPassed});
                break;
            case TileType::EXPLOSIVE:
                Vec2 adjacents[4];
                adjacents[0] = {x - 1, y};
                adjacents[1] = {x + 1, y};
                adjacents[2] = {x, y - 1};
                adjacents[3] = {x, y + 1};

                WorldActionQueue.push({[this, x, y]()
                                       { this->ChangeTile(x, y, TileStates[AIR]); }, TicksPassed});
                for (int i = 0; i < 4; i++)
                {
                    Vec2 point = adjacents[i];
                    int x = point.x, y = point.y;

                    if (IsInBounds(x, y) && tiles[x][y].TileType != AIR)
                    {
                        WorldActionQueue.push({[this, x, y, &player]()
                                               { this->MineTile(x, y, player); }, TicksPassed + 5});
                    }
                }
                break;
            default:
                throw std::runtime_error("Invalid tile\n");
                break;
            }
        }
    }
}