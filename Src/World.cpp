#include "World.h"

using namespace std;

World::World(int width, int height)
{
    TileStates[AIR] = {TileType::AIR, 0, true, true};
    TileStates[STONE] = {TileType::STONE, 10, false, false};
    TileStates[DENSE_STONE] = {TileType::DENSE_STONE, 50, false, false};
    TileStates[GOLD] = {TileType::GOLD, 5, false, false};
    TileStates[EXPLOSIVE] = {TileType::EXPLOSIVE, 1, true, false};
    TileStates[BARRIER] = {TileType::BARRIER, 1, false, true};

    this->Width = width;
    this->Height = height;
    this->tiles = new TileState *[Width];

    ValueNoise2D noiseGen(time(NULL));

    float yStrength = 1.1f;

    for (int i = 0; i < Width; i++)
    {
        this->tiles[i] = new TileState[Height];
        for (int j = 0; j < Height; j++)
        {
            float noise = noiseGen.noise(i * 0.125f, j * 0.125f);
            float yNorm = (float)j/(float)Height; //prioritize denser rock towards greater y

            float value = noise + (yNorm * yStrength);

            value = std::clamp(value, -1.0f, 1.0f);

            TileType idx;
            if (value < -0.50f)
                idx = GOLD;
            else if (value < -0.00f)
                idx = DENSE_STONE;
            else if (value < 0.50f)
                idx = STONE;
            else
                idx = AIR;
            this->tiles[i][j] = TileStates[idx];
        }
    }
    SetBorder();
}
World::~World()
{
    for (int i = 0; i < Width; i++)
    {
        delete[] tiles[i];
    }
    delete[] tiles;
}

void World::GenerateVein(int x, int y, int count, TileType tileType)
{
}

void World::SetBorder() {
    for (int i = 0; i < Width; i++)
    {
        for (int j = 0; j < Height; j++) {
            if (i == 0 || i == Width - 1 || j == 0 || j == Height - 1)
                tiles[i][j] = TileStates[BARRIER];
        }
    }
}

void World::Update(Uint64 tickCount)
{
    this->TickCount = tickCount;
    while (!WorldActionQueue.empty() && WorldActionQueue.top().TickExecuted <= TickCount)
    {
        WorldAction worldAction = WorldActionQueue.top();
        WorldActionQueue.pop();

        worldAction.Action();
    }
    //TickCount++;
}

void World::ChangeTile(int x, int y, TileType TileType)
{
    this->tiles[x][y] = TileStates[TileType];
}

void World::MineTile(int x, int y, int strength, Player &player)
{
    if (IsInBounds(x, y))
    {
        TileState &tileState = tiles[x][y];

        tileState.Health -= strength;
        if (tileState.Health <= 0)
        {

            switch (tileState.TileType)
            {
            case TileType::AIR:
                return;
            case TileType::STONE:
                WorldActionQueue.push({[this, x, y]()
                                       { this->ChangeTile(x, y, AIR); }, TickCount});
                break;
            case TileType::GOLD:
                WorldActionQueue.push({[this, x, y, &player]()
                                       {player.Score++; 
                                        this->ChangeTile(x, y, AIR); }, TickCount});
                break;
            case TileType::EXPLOSIVE:
                Vec2 adjacents[4];
                adjacents[0] = {x - 1, y};
                adjacents[1] = {x + 1, y};
                adjacents[2] = {x, y - 1};
                adjacents[3] = {x, y + 1};

                WorldActionQueue.push({[this, x, y]()
                                       { this->ChangeTile(x, y, AIR); }, TickCount});
                for (int i = 0; i < 4; i++)
                {
                    Vec2 point = adjacents[i];
                    int x = point.x, y = point.y;

                    if (IsInBounds(x, y) && tiles[x][y].TileType != AIR)
                    {
                        WorldActionQueue.push({[this, x, y, &player]()
                                               { this->MineTile(x, y, 10, player); }, TickCount + 5});
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