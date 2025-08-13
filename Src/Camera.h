#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2\SDL.h>
#include "Entity/Player.h"
#include <stdexcept>

struct Camera
{
private:
public:
    Player *_Player;
    Vec2F &Position;
    Vec2 MinCoords;
    Vec2 MaxCoords;
    Vec2 TileCounts;
    int TileLength;
    Vec2 TileOffset;

    Vec2 MouseCoords;
    Vec2 MouseDeltas;

    Camera(Player *player, Vec2 windowDim);
    ~Camera();
    Vec2F ScreenToWorld(Vec2 screenCoords)
    {
        throw std::runtime_error("Unimplemented\n");
    }
    Vec2 WorldToScreen(Vec2F worldCoords)
    {
        return ((worldCoords - MinCoords.ToVec2F()) * TileLength).ToVec2() - TileOffset;
    }
    void Resize();
    void Update();
};

#endif