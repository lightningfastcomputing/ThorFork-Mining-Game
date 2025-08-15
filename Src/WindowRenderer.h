#ifndef WINDOWRENDERER_H
#define WINDOWRENDERER_H

#include <iostream>
#include <format>
#include <string>
#include "World.h"
#include "Entity/Player.h"
#include "Entity/Explosive.h"
#include "Camera.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class WindowRenderer
{
private:
    Uint64 TickCount;

    static constexpr int CIRCLE_SEGMENTS = 300;

    SDL_Window *Window = nullptr;
    SDL_Renderer *Renderer = nullptr;
    SDL_Texture *TileTextures[TILETYPE_COUNT] = {nullptr};
    SDL_Texture *EntityTextures[ENTITYTYPE_COUNT] = {nullptr};
    TTF_Font *TextFont = NULL;

    Camera &_Camera;
    const World &_World;

    bool **Discovered;

    bool Debug, Fullscreen, GlobalView;

    Vec2 WindowDimensions; // window width and height

    void Init_Display(const char *windowTitle);
    void GlobalDrawWorld();
    inline void DrawWorld();
    inline void DrawEntities();
    inline void DebugInfo();
    inline void HighlightTarget();
    inline void DrawPlayerBoundingBox();
    inline void DrawPlayerCollisionBox();
    inline void DrawPlayerVector();
    void DrawPlayerReach();
    void DrawCursor();
    void RadialDiscover();
    void Encircle(Vec2F pos, float radius);
    void RenderFrame();
    void ClearFrame();

public:
    bool Running;

    WindowRenderer(const World &world, Camera &camera, int width, int height);
    ~WindowRenderer();
    void UpdateWindow();
    void ToggleGlobalView();
    void ToggleFullScreen();
    bool IsDiscovered(int x, int y);
    void Outline(float x, float y, float w, float h);
    void ToggleDebug();
    void Reveal();
    void ToggleCursorMode();
    void Update(Uint64 tickCount);
};
#endif