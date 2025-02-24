#ifndef GAME_H
#define GAME_H

#include "WindowRenderer.h"
#include "InputManager.h"
#include "EntityManager.h"
#include <SDL2/SDL.h>

class Game
{
private:
    World &_World;
    InputManager &_InputManager;
    WindowRenderer &_WindowRenderer;
    EntityManager &_EntityManager;

public:
    Uint64 FrameRate;
    Game(Uint64 frameRate, World &world, EntityManager &entityManager, InputManager &inputManager, WindowRenderer &windowRenderer);
    ~Game();
    void Start();
};

#endif
