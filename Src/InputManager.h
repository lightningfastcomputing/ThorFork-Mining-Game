#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include "World.h"
#include "Player.h"
#include "WindowRenderer.h"
#include "SoundManager.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <functional>

enum Action
{
    REVEAL,
    TOGGLE_DEBUG,
    TOGGLE_FULLSCREEN,
    EXIT,
    ACTIONS_COUNT
};

enum Movement
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    MOVEMENT_COUNT
};

struct Input
{
    SDL_Scancode ScanCode;
    Uint64 MillisecondCooldown;
    Uint64 LastTimePressed;
    std::function<void()> Action;
};

struct MouseInputs
{
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
    SoundManager &_SoundManager;

    Input MovementInputs[MOVEMENT_COUNT];
    Input ActionInputs[ACTIONS_COUNT];
    struct MouseInputs MouseInputs;
    const Uint8 *Keys;
    void HandleMouseInput();
    void HandleMovement();
    void PollAndUpdate(int actionIndex);

    void Reveal();
    void ToggleFullScreen();
    void ToggleDebug();
    void Exit();

public:
    InputManager();
    InputManager(World &world, Player &player, WindowRenderer &renderer, SoundManager &SoundManager);
    ~InputManager();
    void ManageInput();
    bool Running;
};
#endif
