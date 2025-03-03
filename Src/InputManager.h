#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include "World.h"
#include "Player.h"
#include "WindowRenderer.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <functional>

struct Input
{
    SDL_Scancode ScanCode;
    Uint64 MillisecondCooldown;
    Uint64 LastTimePressed;
    std::function<void()> Action;
};

struct MouseInputs {
    Uint64 LeftCooldown;
    Uint64 LeftLastTimePressed;
    Uint64 RightCooldown;
    Uint64 RightLastTimePressed;
};

class InputManager
{
private:
    World &_World;
    Player &_Player;
    WindowRenderer &_Renderer;
    Input MovementInputs[4];
    Input ActionInputs[10];
    struct MouseInputs MouseInputs;
    const Uint8 *Keys;
    void HandleMouseInput();
    void HandleMovement();
    void PollAndUpdate(int actionIndex);
    void UpdatePlayer();

    void Reveal();
    void ToggleFullScreen();
    void ToggleDebug();
    void Exit();

public:
    InputManager();
    InputManager(World &world, Player &player, WindowRenderer &renderer);
    ~InputManager();
    void ManageInput();
    bool Running;
};
#endif
