#ifndef WINDOWRENDERER_H
#define WINDOWRENDERER_H

#include <iostream>
#include <format>
#include <string>
#include "World.h"
#include "Player.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class WindowRenderer
{
private:
    Uint64 TickCount;
    
    SDL_Window *Window = nullptr;
    SDL_Renderer *Renderer = nullptr;
    SDL_Texture *Textures[TILETYPE_COUNT] = {nullptr};

    TTF_Font *TextFont = nullptr;

    const World &_World;
    bool** Discovered;
    std::vector<Player *> &_Players;

    bool Debug, Fullscreen;

    int TileLength;

    Vec2 WindowDimensions; // window width and height
    Vec2 TileCounts;       // number of tiles, horizontal and vertical, that fit on the screen

    Vec2 TileOffset;       // number of pixels offset from the middle of a tile, ranges from 0 to tilelength

    Vec2 MinCoordinates;   // Minimum world coordinates that the window renders
    Vec2 MaxCoordinates;   // Maximum world coordinates that the window renders

    void Init_Display(const char *windowTitle);
    inline void DrawWorld();
    inline void DrawPlayers();
    inline void DebugInfo();
    inline void DrawAndStoreSelectedTile();
    inline void DrawPlayerBoundingBox();
    inline void DrawPlayerCollisionBox();
    inline void DrawPlayerVector();
    void DrawPlayerReach();
    void DrawCursor();
    void RadialDiscover();
    void RenderFrame();
    void ClearFrame();

public:
    Player *_Player = nullptr;

    bool Running;
    Vec2 MouseScreen; // coordinates of the mouse on the screen
    Vec2 MouseWorld;  // coordinates of the mouse relative to the world

    WindowRenderer(const World &world, Player *player, std::vector<Player *> &players, int width, int height);
    ~WindowRenderer();
    void UpdateWindow();
    void ToggleFullScreen();
    bool IsDiscovered(int x, int y);
    void OutlineTile(int x, int y);
    void ToggleDebug();
    void Reveal();
    void Update(Uint64 tickCount);
};
#endif