#include "Camera.h"
Camera::Camera(Player *player, Vec2 windowDim) : _Player(player), Position(_Player->Center)
{
    TileLength = 48;
    TileCounts = windowDim.ToVec2F() / TileLength;
    WindowDim = windowDim;
    MouseCoords = WorldToScreen(player->Center).ToVec2();
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
    //TileOffset = ((Position - Position.ToVec2().ToVec2F()) * TileLength).ToVec2();
    MinCoord = Position - (TileCounts / 2);
    MaxCoord = MinCoord + TileCounts;
}