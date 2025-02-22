#ifndef GAME_H
#define GAME_H

#include "WindowRenderer.h"
#include "InputManager.h"
#include "Player.h"
#include "World.h"
#include <SDL2/SDL.h>

class Game
{
private:
    World *_World;
    InputManager *_InputManager;
    Player *_Player;
    WindowRenderer *_WindowRenderer;
    void Cycle();
public:
    Uint64 FrameRate;
    Game(Uint64 FrameRate);
    ~Game();
    void Start();
};

#endif
