#ifndef GAME_H
#define GAME_H

#include "WindowRenderer.h"
#include "InputManager.h"
#include "EntityManager.h"
#include <SDL2/SDL.h>

class Game
{
private:
    Uint64 FrameRate;
    World &_World;
    EntityManager &_EntityManager;
    InputManager &_InputManager;
    WindowRenderer &_WindowRenderer;
    bool Running;

public:
    Game(Uint64 frameRate, World &world, EntityManager &entityManager, InputManager &inputManager, WindowRenderer &windowRenderer);
    ~Game();
    void Start();
};

#endif
