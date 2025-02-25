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
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    SDL_Texture *Textures[4] = {NULL};

    World &_World;
    Player &_Player;

    bool **Discovered, Debug;
    int Width, Height;
    int TileLength;
    float xOffset, yOffset;
    void Init_Display(const char *windowTitle);
    void DrawPlayer();
    void DrawAndStoreSelectedTile(int minX, int minY);
    void DrawPlayerBoundingBox(int tileLength, int xRem);
    void DrawPlayerCollisionBox(int minX, int minY);
    void DrawPlayerVector(int tileLength, int xRem);

public:
    bool Running;
    int MouseX, MouseY;
    int MouseWorldX, MouseWorldY;
    WindowRenderer();
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