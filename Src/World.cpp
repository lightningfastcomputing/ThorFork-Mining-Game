#include "World.h"

World::World(int width, int height, SoundManager &soundManager) : _SoundManager(soundManager), Width(width), Height(height)
{
    InitTiles();
    InitEntities();
}

void World::InitTiles()
{
    TileStates[AIR] = {TileType::AIR, 0, true, true};
    TileStates[STONE_FLOOR] = {TileType::STONE_FLOOR, 0, true, true};
    TileStates[STONE] = {TileType::STONE, 10, false, false};
    TileStates[DENSE_STONE] = {TileType::DENSE_STONE, 20, false, false};
    TileStates[GOLD] = {TileType::GOLD, 5, false, false};
    TileStates[BARRIER] = {TileType::BARRIER, 1, false, true};

    ValueNoise2D noiseGen(time(NULL));
    float yStrength = 1.1f;
    int yThreshold = Height / 3;

    Tiles = new TileState *[Width];
    for (int i = 0; i < Width; i++)
    {
        Tiles[i] = new TileState[Height];
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
            Tiles[i][j] = TileStates[idx];
        }
    }
    SetBorder();
}

void World::InitEntities()
{
    _Entities.reserve(100);
}

World::~World()
{
    for (int i = 0; i < Width; i++)
    {
        delete[] Tiles[i];
    }
    delete[] Tiles;

    for (Entity *e : _Entities)
    {
        delete e;
    }
}

void World::SetBorder()
{
    for (int i = 0; i < Width; i++)
    {
        for (int j = 0; j < Height; j++)
        {
            if (i == 0 || i == Width - 1 || j == 0 || j == Height - 1)
                Tiles[i][j] = TileStates[BARRIER];
        }
    }
}

void World::Explosion(Explosive *e)
{
    Vec2F pos = e->Center;
    float radius = e->ExplosionRadius;
    std::vector<Vec2> tiles = GetTiles(pos, radius);
    std::vector<Entity *> entities = GetEntities(pos, radius);

    for (Entity *e : entities)
    {
        KillEntity(e);
    }
    for (Vec2 v : tiles)
    {
        MineTile(v.x, v.y, 50, false);
    }

    _SoundManager.PlaySound(EXPLOSION);
}

std::vector<Vec2> World::GetTiles(Vec2F pos, float radius)
{
    std::vector<Vec2> tiles;
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
            Vec2 tile = {x, y};
            if (Tiles[x][y].TileType != AIR && std::find(tiles.begin(), tiles.end(), tile) == tiles.end())
                tiles.push_back(tile);
            if ((Tiles[x][y].TileType == TileType::BARRIER))
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
    return tiles;
}

std::vector<Entity *> World::GetEntities(Vec2F pos, float radius)
{
    std::vector<Entity *> entities;

    for (Entity *e : _Entities)
    {
        if (Utils::Distance(pos, e->Center) <= radius)
            entities.push_back(e);
    }
    return entities;
}

void World::ChangeTile(int x, int y, TileType TileType)
{
    Tiles[x][y] = TileStates[TileType];
}

void World::MineTile(int x, int y, int strength, bool audible)
{
    float EPSILON = Entity::EPSILON;
    if (InBounds({x, y}))
    {
        TileState &tileState = Tiles[x][y];
        if (tileState.Passable || tileState.Indestrucible)
        {
            return;
        }
        tileState.Health -= strength;
        if (audible)
            _SoundManager.PlaySound(PICKAXE_STRIKE);

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
            case TileType::GOLD:
            {

                unsigned int type = tileState.TileType;
                WorldActionQueue.push({[this, x, y, type, EPSILON]()
                                       {
                                        this->ChangeTile(x, y, STONE_FLOOR);
                                        //this->_SoundManager.PlaySound(ROCK_CRUMBLE);
                                        Vec2F pos = {x + 0.5f,y + 0.5f};
                                        Vec2F dim = Vec2F(0.6f, 0.6f) + Vec2F(0.1f * (rand()%4), 0.1f * (rand()%4)) - Vec2F(EPSILON, EPSILON); 
                                        if (rand()%5 == 0)
                                            this->SpawnChunk(pos, dim, (TileType)type); }, TickCount});
                break;
            }
            default:
                throw std::runtime_error("Invalid tile\n");
                break;
            }
        }
    }
}

void World::UpdateEntities()
{

    for (Entity *p : _Entities)
    {
        // parent takes care of movement
        if (p->Parent)
        {
            continue;
        }

        if (p->Type == MINECART)
        {
            Minecart *mc = static_cast<Minecart *>(p);
            Entity *track = FindEntity(mc->Center, MINECART_TRACK);

            if (track)
            {
                mc->DragCoefficient = 0.99f;
                mc->MaxVelocity = 0.5f;
                mc->OnTrack = true;
                mc->Position.x = track->Position.x - (mc->Dimensions.x - track->Dimensions.x) / 2;
                mc->UpdateTileBounds();
                mc->Acceleration.x = 0;
            }
            else
            {
                mc->OnTrack = false;
            }
        }

        Vec2F &velocity = p->Velocity;

        p->Velocity += p->Acceleration;
        p->Acceleration = {0, 0};
        if (p->Velocity.Magnitude() > p->MaxVelocity)
        {
            p->Velocity.Normalize();
            p->Velocity = p->Velocity * p->MaxVelocity;
        }

        if (p->Velocity.Magnitude() == 0)
        {
            continue;
        }

        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        float &x = p->Position.x, &y = p->Position.y, w = p->Dimensions.x, h = p->Dimensions.y;
        Vec2F positionDelta = {x, y};
        float EPSILON = p->EPSILON;

        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (velocity.x != 0)
        {
            float collisionVelocity = !p->Elastic ? 0 : -velocity.x * 0.5f;
            int xIdx = velocity.x < 0 ? xStart : xEnd;
            float xSnap = velocity.x < 0 ? (float)xStart + 1 + EPSILON : (float)xEnd - w - EPSILON;

            for (int i = yStart; i <= yEnd; i++)
            {

                if (!Tiles[xIdx][i].Passable)
                {
                    x = xSnap;
                    velocity.x = collisionVelocity;
                }
            }

            for (Entity *other : _Entities)
            {
                if (!p->CanCollide(other) || other == p)
                    continue;

                float xSnap = velocity.x < 0 ? other->Position.x + other->Dimensions.x + EPSILON
                                             : other->Position.x - w - EPSILON;

                SDL_FRect entityRect = p->ToFRect(), otherRect = other->ToFRect();
                if (SDL_HasIntersectionF(&entityRect, &otherRect))
                {
                    x = xSnap;
                    velocity.x = collisionVelocity;
                }
            }
        }

        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (velocity.y != 0)
        {
            float collisionVelocity = !p->Elastic ? 0 : -velocity.y * 0.5f;
            int yIdx = velocity.y < 0 ? yStart : yEnd;
            float ySnap = velocity.y < 0 ? (float)yStart + 1 + EPSILON : (float)yEnd - h - EPSILON;

            for (int i = xStart; i <= xEnd; i++)
            {
                if (!Tiles[i][yIdx].Passable)
                {
                    y = ySnap;
                    velocity.y = collisionVelocity;
                }
            }
            for (Entity *other : _Entities)
            {
                if (!p->CanCollide(other) || other == p)
                    continue;

                float ySnap = velocity.y < 0 ? other->Position.y + other->Dimensions.y + EPSILON : other->Position.y - h - EPSILON;

                SDL_FRect entityRect = p->ToFRect(), otherRect = other->ToFRect();
                if (SDL_HasIntersectionF(&entityRect, &otherRect))
                {
                    y = ySnap;
                    velocity.y = collisionVelocity;
                }
            }
        }

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        p->Center = p->Position + (p->Dimensions * 0.5f);
        p->Velocity = p->Velocity * p->DragCoefficient; // friction

        if (p->Velocity.Magnitude() < 0.001f)
            p->Velocity = {0, 0};

        positionDelta.x = x - positionDelta.x;
        positionDelta.y = y - positionDelta.y;

        if (p->Children.size() > 0)
        {
            for (Entity *e : p->Children)
            {
                e->Position += positionDelta;
                e->UpdateTileBounds();
            }
        }

        // if (p->Type == PLAYER)
        // {
        //     Player *player = static_cast<Player *>(p);
        //     player->Target.x += positionDelta.x;
        //     player->Target.y += positionDelta.y;
        // }
    }
}

void World::AddPlayer(Player *player)
{
    player->Position = {(float)Width / 2, (float)Height / 2};
    player->UpdateTileBounds();

    std::vector<Vec2> tiles = GetTiles(player->Center, player->MiningRadius);
    for (Vec2 t : tiles)
    {
        ChangeTile(t.x, t.y, AIR);
    }
    std::vector<Entity *>::iterator it = std::lower_bound(_Entities.begin(), _Entities.end(), player,
                                                          [](Entity *a, Entity *b)
                                                          { return a->Type < b->Type; });
    _Entities.insert(it, player);
}

Explosive *World::SpawnExplosive(float x, float y)
{
    Explosive *e = new Explosive(x, y, 1.1f, 5.0f);

    std::vector<Entity *>::iterator it = std::lower_bound(_Entities.begin(), _Entities.end(), e,
                                                          [](Entity *a, Entity *b)
                                                          { return a->Type < b->Type; });
    _Entities.insert(it, e);
    return e;
}

Chunk *World::SpawnChunk(Vec2F pos, Vec2F dim, TileType type)
{
    Chunk *c = new Chunk(pos.x, pos.y, dim.x, dim.y, type);
    std::vector<Entity *>::iterator it = std::lower_bound(_Entities.begin(), _Entities.end(), c,
                                                          [](Entity *a, Entity *b)
                                                          { return a->Type < b->Type; });
    _Entities.insert(it, c);
    return c;
}

Entity *World::SpawnEntity(Vec2F pos, EntityType type)
{
    Entity *e = nullptr;
    if (FindEntity(pos, type))
        return e;

    switch (type)
    {
    case MINECART:
        e = new Minecart(pos.x, pos.y);
        break;
    case MINECART_TRACK:
    {

        Vec2F centerVec = pos.Floor() + Vec2F{0.5f, 0.5f};
        e = new MinecartTrack(centerVec.x, centerVec.y);
        break;
    }
    default:
        break;
    }

    if (e)
    {
        std::vector<Entity *>::iterator it = std::lower_bound(_Entities.begin(), _Entities.end(), e,
                                                              [](Entity *a, Entity *b)
                                                              { return a->Type < b->Type; });
        _Entities.insert(it, e);
    }
    return e;
}

void World::KillEntity(Entity *&entity)
{
    if (!entity || !entity->Killable || entity->Type == PLAYER)
        return;

    _Entities.erase(std::remove(_Entities.begin(), _Entities.end(), entity), _Entities.end());

    switch (entity->Type)
    {
    case DYNAMITE:
    {
        Explosive *e = static_cast<Explosive *>(entity);
        Explosion(e);
        break;
    }
    case CHUNK:
    {
        _SoundManager.PlaySound(ROCK_CRUMBLE);
        break;
    }

    default:
        break;
    }

    delete entity;
    entity = nullptr;
}

// use type = -1 if you just want to find the first one regardless of type
Entity *World::FindEntity(Vec2F pos, int type) const
{
    SDL_FPoint point = {pos.x, pos.y};
    if (type < 0 || type >= ENTITYTYPE_COUNT)
    {
        for (Entity *e : std::ranges::reverse_view(_Entities))
        {
            SDL_FRect rect = e->ToFRect();
            if (SDL_PointInFRect(&point, &rect))
            {
                return e;
            }
        }
    }
    else
    {
        for (Entity *e : std::ranges::reverse_view(_Entities))
        {
            SDL_FRect rect = e->ToFRect();
            if (SDL_PointInFRect(&point, &rect) && e->Type == type)
            {
                return e;
            }
        }
    }
    return nullptr;
}

void World::Update(Uint64 tickCount)
{
    this->TickCount = tickCount;
    UpdateEntities();
    while (!WorldActionQueue.empty() && WorldActionQueue.top().TickExecuted <= TickCount)
    {
        WorldAction worldAction = WorldActionQueue.top();
        WorldActionQueue.pop();

        worldAction.Action();
    }
}