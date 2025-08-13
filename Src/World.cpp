#include "World.h"

World::World(int width, int height, SoundManager &soundManager) : _SoundManager(soundManager)
{
    TileStates[AIR] = {TileType::AIR, 0, true, true};
    TileStates[STONE_FLOOR] = {TileType::STONE_FLOOR, 0, true, true};
    TileStates[STONE] = {TileType::STONE, 10, false, false};
    TileStates[DENSE_STONE] = {TileType::DENSE_STONE, 20, false, false};
    TileStates[GOLD] = {TileType::GOLD, 5, false, false};
    TileStates[EXPLOSIVE] = {TileType::EXPLOSIVE, 1, true, false};
    TileStates[BARRIER] = {TileType::BARRIER, 1, false, true};

    this->Width = width;
    this->Height = height;
    this->tiles = new TileState *[Width];

    ValueNoise2D noiseGen(time(NULL));

    float yStrength = 1.1f;
    int yThreshold = Height / 3;

    for (int i = 0; i < Width; i++)
    {
        this->tiles[i] = new TileState[Height];
        for (int j = 0; j < Height; j++)
        {
            float noise = noiseGen.noise(i * 0.125f, j * 0.125f);
            float yNorm = (float)j / (float)Height; // prioritize denser rock towards greater y

            float value = noise + (yNorm * yStrength);

            value = std::clamp(value, -1.0f, 1.0f);

            TileType idx;
            if (value < -0.50f)
                idx = GOLD;
            else if (value < -0.00f)
                idx = DENSE_STONE;
            else if (value < 0.50f)
                idx = STONE;
            else if (j < yThreshold)
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

void World::SetBorder()
{
    for (int i = 0; i < Width; i++)
    {
        for (int j = 0; j < Height; j++)
        {
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
    // TickCount++;
}

void World::Explosion(Vec2F pos, float radius)
{
    int numRays = (int)(2 * PI * radius) + 1; // aproximation of how many rays are needed to get a clean discovery radius
    float dTheta = (2 * PI) / numRays;
    float theta = 0;

    for (int i = 0; i < numRays; i++)
    {
        Vec2F endPos = {pos.x + (radius * SDL_cosf(theta)), pos.y + (radius * SDL_sinf(theta))};

        int x = (int)pos.x;
        int y = (int)pos.y;
        int endX = (int)(endPos.x);
        int endY = (int)(endPos.y);

        float dx = endPos.x - pos.x;
        float dy = endPos.y - pos.y;

        int stepX = (dx > 0) ? 1 : (dx < 0) ? -1
                                            : 0;
        int stepY = (dy > 0) ? 1 : (dy < 0) ? -1
                                            : 0;

        float tDeltaX = (stepX != 0) ? SDL_fabsf(1.0f / dx) : 1e6;
        float tDeltaY = (stepY != 0) ? SDL_fabsf(1.0f / dy) : 1e6;

        float nextTileBoundaryX = (stepX > 0) ? (SDL_floorf(pos.x) + 1.0f) : (SDL_floorf(pos.x));
        float nextTileBoundaryY = (stepY > 0) ? (SDL_floorf(pos.y) + 1.0f) : (SDL_floorf(pos.y));

        float tMaxX = (stepX != 0) ? SDL_fabsf((nextTileBoundaryX - pos.x) / dx) : 1e6;
        float tMaxY = (stepY != 0) ? SDL_fabsf((nextTileBoundaryY - pos.y) / dy) : 1e6;

        while (x != endX || y != endY)
        {
            MineTile(x, y, 50, nullptr);
            if ((tiles[x][y].TileType == TileType::BARRIER))
                break;

            if (tMaxX < tMaxY)
            {
                tMaxX += tDeltaX;
                x += stepX;
            }
            else
            {
                tMaxY += tDeltaY;
                y += stepY;
            }
        }
        theta += dTheta;
    }
}

void World::ChangeTile(int x, int y, TileType TileType)
{
    this->tiles[x][y] = TileStates[TileType];
}

void World::MineTile(int x, int y, int strength, Player *player)
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
            case TileType::STONE_FLOOR:
            case TileType::BARRIER:
                break;
            case TileType::STONE:
            case TileType::DENSE_STONE:
                WorldActionQueue.push({[this, x, y]()
                                       { this->ChangeTile(x, y, STONE_FLOOR); }, TickCount});
                break;
            case TileType::GOLD:
                WorldActionQueue.push({[this, x, y, player]()
                                       {if (player)
                                            player->Score++; 
                                        this->ChangeTile(x, y, STONE_FLOOR); }, TickCount});
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
                        WorldActionQueue.push({[this, x, y, player]()
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