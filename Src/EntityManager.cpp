#include "EntityManager.h"

EntityManager::EntityManager(World &world, Player &player) : _World(world), _Player(player)
{
    _Player.BoundingBox.x = _World.Width / 2;
    _Player.BoundingBox.y = _World.Height / 2;

    _Player.Center = {_Player.BoundingBox.x + _Player.HalfDimensions.x, _Player.BoundingBox.y + _Player.HalfDimensions.y};

    _Player.DiscoveredTiles = new bool *[_World.Width];
    for (int i = 0; i < _World.Width; i++)
    {
        _Player.DiscoveredTiles[i] = new bool[_World.Height];
        for (int j = 0; j < _World.Height; j++)
            _Player.DiscoveredTiles[i][j] = false;
    }
}

EntityManager::~EntityManager()
{
    for (int i = 0; i < _World.Width; i++)
    {
        delete[] _Player.DiscoveredTiles[i];
    }
    delete[] _Player.DiscoveredTiles;
}

void EntityManager::Update()
{
    UpdatePlayerPosition();
    PlayerRadialDiscover();
    PlayerTryMine();
}

// sets ability to mine only if center has line of sight to the tile
void EntityManager::PlayerTryMine()
{
    Vec2 targetTile = {(int)_Player.Target.x, (int)_Player.Target.y};

    // check if inside player
    SDL_Rect collisionRect = {_Player.xStart, _Player.yStart, (_Player.xEnd - _Player.xStart + 1), (_Player.yEnd - _Player.yStart + 1)};
    if (SDL_PointInRect((SDL_Point *)&targetTile, &collisionRect))
    {
        _Player.CanMine = false;
        return;
    }

    // check if in bounds or if discovered
    if (!_World.IsInBounds(targetTile) || !_Player.DiscoveredTiles[targetTile.x][targetTile.y])
    {
        _Player.CanMine = false;
        return;
    }

    // check for line of sight
    Vec2F dir = _Player.Target - _Player.Center; // vector toward mouse coordinates
    float distance = dir.Magnitude();
    if (distance > _Player.MiningRadius)
    {
        _Player.CanMine = false;
        return;
    }

    dir.Normalize();
    Vec2F rayPos = _Player.Center;

    for (float t = 0; t < distance; t += 0.2f)
    {
        rayPos = _Player.Center + (dir * t);

        Vec2 tile = {(int)rayPos.x, (int)rayPos.y};

        bool isTarget = (tile.x == targetTile.x) && (tile.y == targetTile.y);
        if (isTarget)
        {
            // prevent pinhole diagonal tile mining: if adjacent tiles are stone, then mining is prevented
            Vec2 adj1 = {tile.x + (dir.x > 0 ? -1 : 1), tile.y};
            Vec2 adj2 = {tile.x, tile.y + (dir.y > 0 ? 1 : -1)};
            if (_World.tiles[adj1.x][adj1.y] != AIR && _World.tiles[adj2.x][adj2.y] != AIR)
            {
                _Player.CanMine = false;
                return;
            }
            else
            {
                _Player.CanMine = true;
                return;
            }
        }
        else if (_World.tiles[tile.x][tile.y] != AIR)
        {
            _Player.CanMine = false;
            return;
        }
    }
}

// space and computation tradeoff: use more rays to parallelize the computation at the cost of space
// or do every 2 frames, or every 3, whatever keeps the illusion
void EntityManager::PlayerRadialDiscover()
{
    Ray ray(_Player.Center, 0, _Player.DiscoverRadius);

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
                _Player.DiscoveredTiles[tile.x][tile.y] = true;

                if (_World.tiles[tile.x][tile.y] != AIR || !_World.IsInBounds(tile.x, tile.y))
                {
                    break; // once the ray hits a wall or OOB, stop iterating
                }
            }
        }
        theta += dTheta;
    }
}

void EntityManager::UpdatePlayerPosition()
{
    int &xStart = _Player.xStart;
    int &xEnd = _Player.xEnd;
    int &yStart = _Player.yStart;
    int &yEnd = _Player.yEnd;

    Vec2F velocity = _Player.Velocity;
    float &x = _Player.BoundingBox.x, &y = _Player.BoundingBox.y, w = _Player.BoundingBox.w, h = _Player.BoundingBox.h;
    float EPSILON = _Player.EPSILON;

    if (velocity.x != 0)
    {
        x += velocity.x;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (xStart < 0)
        {
            x = EPSILON;
        }
        else if (xEnd > _World.Width - 1)
        {
            x = _World.Width - w - EPSILON;
        }
        else
        {
            for (int i = yStart; i <= yEnd; i++)
            {
                // West Collision
                if (_World.tiles[xStart][i] != AIR)
                {
                    x = (float)xStart + 1 + EPSILON;
                    break;
                }
                // East Collision
                else if (_World.tiles[xEnd][i] != AIR)
                {
                    x = (float)xEnd - w - EPSILON;
                    break;
                }
            }
        }
    }

    if (velocity.y != 0)
    {
        y += velocity.y;

        xStart = (int)SDL_floorf(x);
        yStart = (int)SDL_floorf(y);
        xEnd = (int)SDL_floorf(x + w);
        yEnd = (int)SDL_floorf(y + h);

        if (yStart < 0)
        {
            y = EPSILON;
        }
        else if (yEnd > _World.Height - 1)
        {
            y = _World.Height - h - EPSILON;
        }
        else
        {
            for (int i = xStart; i <= xEnd; i++)
            {
                if (_World.tiles[i][yStart] != AIR)
                {
                    y = (float)yStart + 1 + EPSILON;
                    break;
                }
                else if (_World.tiles[i][yEnd] != AIR)
                {
                    y = (float)yEnd - h - EPSILON;
                    break;
                }
            }
        }
    }

    xStart = (int)SDL_floorf(x);
    yStart = (int)SDL_floorf(y);
    xEnd = (int)SDL_floorf(x + w);
    yEnd = (int)SDL_floorf(y + h);

    _Player.Center = {_Player.BoundingBox.x + _Player.HalfDimensions.x, _Player.BoundingBox.y + _Player.HalfDimensions.y};
    _Player.Velocity = {0, 0};
}