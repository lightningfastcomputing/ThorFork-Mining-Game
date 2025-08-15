#ifndef GAME_H
#define GAME_H

#include "WindowRenderer.h"
#include "InputManager.h"
#include "SoundManager.h"
#include <SDL2/SDL.h>

class Game
{
private:
    Uint64 FrameRate;
    SDL_Event Event;
    World *_World;
    WindowRenderer *_WindowRenderer;
    SoundManager *_SoundManager;
    InputManager *_InputManager;

    bool Running;
    Uint64 TickCount = 0;

public:
    Game(Uint64 frameRate, Vec2 worldDimensions);
    ~Game();
    void Start();
    void Update();
};

#endif
