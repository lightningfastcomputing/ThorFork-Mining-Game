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
    World &_World;
    EntityManager &_EntityManager;
    InputManager &_InputManager;
    WindowRenderer &_WindowRenderer;
    SoundManager &_SoundManager;
    bool Running;

public:
    Game(Uint64 frameRate, World &world, EntityManager &entityManager, InputManager &inputManager, WindowRenderer &windowRenderer, SoundManager &SoundManager);
    ~Game();
    void Start();
};

#endif
