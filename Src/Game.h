#ifndef GAME_H
#define GAME_H

#include "WindowRenderer.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "SoundManager.h"
#include <SDL2/SDL.h>

class Game
{
private:
    Uint64 FrameRate;
    SDL_Event Event;
    World &_World;
    EntityManager &_EntityManager;
    InputManager &_InputManager;
    WindowRenderer &_WindowRenderer;
    SoundManager &_SoundManager;
    bool Running;
    Uint64 TickCount = 0;

public:
    Game(Uint64 frameRate, World &world, EntityManager &entityManager, InputManager &inputManager, WindowRenderer &windowRenderer, SoundManager &SoundManager);
    ~Game();
    void Start();
    void Update();
};

#endif
