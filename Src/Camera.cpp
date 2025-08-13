#include "Camera.h"
Camera::Camera(Player *player, Vec2 windowDim) : _Player(player), Position(_Player->Center)
{
    TileLength = 32;
    TileCounts = {windowDim.x / TileLength, windowDim.y / TileLength};
    MouseCoords = {-1, -1};
    MouseDeltas = {0, 0};
}
Camera::~Camera()
{
}

void Resize()
{
}

void Camera::Update()
{
    TileOffset = ((Position - Position.ToVec2().ToVec2F()) * TileLength).ToVec2();
    MinCoords = Position.ToVec2() - TileCounts / 2;
    MaxCoords = MinCoords + TileCounts;
}