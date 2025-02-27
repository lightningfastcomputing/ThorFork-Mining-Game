#ifndef WINDOWRENDERER_H
#define WINDOWRENDERER_H

#include "World.h"
#include "Player.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class WindowRenderer
{
private:
    SDL_Window *Window = nullptr;
    SDL_Renderer *Renderer = nullptr;
    SDL_Texture *Textures[4] = {nullptr};

    World &_World;
    Player &_Player;

    bool **Discovered, Debug;

    int ScreenWidth, ScreenHeight;
    int HorizontalTileCount, VerticalTileCount;

    int TileLength;

    float TileXOffset, TileYOffset;
    float PlayerPosXOffset, PlayerPosYOffset;
    float PlayerDimXOffset, PlayerDimYOffset;
    int ScreenXMin, ScreenYMin;

    void Init_Display(const char *windowTitle);
    inline void DrawWorld();
    inline void DrawPlayer();
    inline void DebugInfo();
    inline void DrawAndStoreSelectedTile();
    inline void DrawPlayerBoundingBox();
    inline void DrawPlayerCollisionBox();
    inline void DrawPlayerVector();

public:

    bool Running;
    int MouseX, MouseY;
    int MouseWorldX, MouseWorldY;

    WindowRenderer(World &world, Player &player, int width, int height);
    ~WindowRenderer();

    void Reveal();
    void Discover();
    bool IsDiscovered(int x, int y);
    void RenderFrame();
    void ClearFrame();
    void ToggleDebug();
};
#endif