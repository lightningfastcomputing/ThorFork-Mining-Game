#include "EntityManager.h"

EntityManager::EntityManager(World &world, std::vector<Player*>& players) : _World(world), _Players(players)
{
    for (Player* p : _Players)
    {
        p->BoundingBox.x = rand() %  (_World.Width - 1) + 1;
        p->BoundingBox.y = rand() %  (_World.Height - 1) + 1;

        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;

        xStart = (int)SDL_floorf(x);
        xEnd = (int)SDL_floorf(x + w);
        yStart = (int)SDL_floorf(y);
        yEnd = (int)SDL_floorf(y + h);

        for (int i = xStart; i <= xEnd; i++)
        {
            for (int j = yStart; j <= yEnd; j++)
            {
                _World.ChangeTile(i, j, TileType::AIR);
            }
        }
        p->Center = {x + p->HalfDimensions.x, y + p->HalfDimensions.y};

        
    }
}

EntityManager::~EntityManager()
{
}

void EntityManager::Update(Uint64 deltaTime)
{
    UpdatePlayerPosition();
    PlayerRadialDiscover(deltaTime);
    PlayerTryMine();
}

// sets ability to mine only if center has line of sight to the tile //broken
void EntityManager::PlayerTryMine()
{
    for (Player* p : _Players)
    {
        p->CanMine = true;
    }
    return;
    
    Player* p = _Players[0]; // assuming only one player for now
        
    Vec2 targetTile = {(int)p->Target.x, (int)p->Target.y};

    // check if inside player
    SDL_Rect collisionRect = {p->xStart, p->yStart, (p->xEnd - p->xStart + 1), (p->yEnd - p->yStart + 1)};
    if (SDL_PointInRect((SDL_Point *)&targetTile, &collisionRect))
    {
        p->CanMine = false;
        return;
    }

    // check if in bounds or if discovered
    if (!_World.IsInBounds(targetTile) || !p->DiscoveredTiles[targetTile.x][targetTile.y])
    {
        p->CanMine = false;
        return;
    }

    // check for line of sight
    Vec2F dir = p->Target - p->Center; // vector toward mouse coordinates
    float distance = dir.Magnitude();
    if (distance > p->MiningRadius)
    {
        p->CanMine = false;
        return;
    }

    dir.Normalize();
    Vec2F rayPos = p->Center;

    for (float t = 0; t < distance; t += 0.2f)
    {
        rayPos = p->Center + (dir * t);

        Vec2 tile = {(int)rayPos.x, (int)rayPos.y};

        bool isTarget = (tile.x == targetTile.x) && (tile.y == targetTile.y);
        if (isTarget)
        {
            // prevent pinhole diagonal tile mining: if adjacent tiles are stone, then mining is prevented
            Vec2 adj1 = {tile.x + (dir.x > 0 ? -1 : 1), tile.y};
            Vec2 adj2 = {tile.x, tile.y + (dir.y > 0 ? 1 : -1)};
            if (_World.tiles[adj1.x][adj1.y].TileType != TileType::AIR && _World.tiles[adj2.x][adj2.y].TileType != TileType::AIR)
            {
                p->CanMine = false;
                return;
            }
            else
            {
                p->CanMine = true;
                return;
            }
        }
        else if (_World.tiles[tile.x][tile.y].TileType != TileType::AIR)
        {
            p->CanMine = false;
            return;
        }
    }
}

// space and computation tradeoff: use more rays to parallelize the computation at the cost of space
// or do every 2 frames, or every 3, whatever keeps the illusion
void EntityManager::PlayerRadialDiscover(Uint64 deltaTime)
{
    for (Player* p : _Players) 
    {
        Ray ray(p->Center, 0, p->DiscoverRadius);

        Vec2F rayPos = ray.Origin;

        int numRays = (int)(2 * PI * ray.Length) + 1; // aproximation of how many rays are needed to get a clean discovery radius
        float dTheta = (2 * PI) / numRays;
        float theta = 0;

        for (int i = 0; i < numRays; i++)
        {
            Vec2F dir = {SDL_cosf(theta), SDL_sinf(theta)};

            for (float t = 0; t < ray.Length; t += 0.5f) // advance by half a tile length
            {
                rayPos = ray.Origin + (dir * t);

                Vec2 tile = {(int)rayPos.x, (int)rayPos.y};

                if (_World.IsInBounds(tile.x, tile.y))
                {
                    p->DiscoveredTiles[tile.x][tile.y] = true;

                    if (_World.tiles[tile.x][tile.y].TileType != AIR || !_World.IsInBounds(tile.x, tile.y))
                    {
                        break; // once the ray hits a wall or OOB, stop iterating
                    }
                }
            }
            theta += dTheta;
        }
    }
}

//todo: account for collision with other players 
void EntityManager::UpdatePlayerPosition()
{ 

    for (Player* p : _Players)
    {
        int &xStart = p->xStart, &xEnd = p->xEnd, &yStart = p->yStart, &yEnd = p->yEnd;
        Vec2F velocity = p->Velocity;
        float &x = p->BoundingBox.x, &y = p->BoundingBox.y, w = p->BoundingBox.w, h = p->BoundingBox.h;
        float EPSILON = p->EPSILON;

        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        // West Collision
        if (velocity.x < 0)
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                
                if (_World.tiles[xStart][i].TileType != TileType::AIR)
                {
                    x = (float)xStart + 1 + EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                    x = other->BoundingBox.x + other->BoundingBox.w + EPSILON;
            }
        }
        // East Collision
        else if (velocity.x > 0)
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                
                if (_World.tiles[xEnd][i].TileType != TileType::AIR)
                {
                    x = (float)xEnd - w - EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                    x = other->BoundingBox.x - w - EPSILON;
            }
        }
        
        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        // North Collision
        if (velocity.y < 0) {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (_World.tiles[i][yStart].TileType != TileType::AIR)
                {
                    y = (float)yStart + 1 + EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                {
                    y = other->BoundingBox.y + other->BoundingBox.h + EPSILON;
                }
            }
            
        }
        // South Collision
        else if (velocity.y > 0)
        {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (_World.tiles[i][yEnd].TileType != TileType::AIR)
                {
                    y = (float)yEnd - h - EPSILON;
                    break;
                }
            }

            for (Player* other : _Players)
            {
                if (other == p) continue;

                if (SDL_HasIntersectionF(&p->BoundingBox, &other->BoundingBox))
                {
                    y = other->BoundingBox.y - h - EPSILON;
                }
            }
        }

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        p->Center = {p->BoundingBox.x + p->HalfDimensions.x, p->BoundingBox.y + p->HalfDimensions.y};
        p->Velocity = {0, 0};
    }
}

void EntityManager::EntityWorldCollisionCheck()
{

}

void EntityManager::EntityEntityCollisionCheck()
{

}