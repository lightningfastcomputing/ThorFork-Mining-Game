#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include "World.h"
#include "Entity/Player.h"
#include "WindowRenderer.h"
#include "SoundManager.h"
#include "Camera.h"
#include "Utils.h"
#include <SDL2/SDL.h>
#include <functional>

enum Action
{
    REVEAL,
    TOGGLE_DEBUG,
    TOGGLE_FULLSCREEN,
    EXIT,
    SWITCH_PLAYER_0,
    SWITCH_PLAYER_1,
    SWITCH_PLAYER_2,
    SWITCH_PLAYER_3,
    SWITCH_PLAYER_4,
    SWITCH_PLAYER_5,
    SWITCH_PLAYER_6,
    SWITCH_PLAYER_7,
    SWITCH_PLAYER_8,
    TOGGLE_CURSOR_MODE,
    TOGGLE_VIEW,
    GRAB_RELEASE,
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
    int ScanCode;
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
    Player *_Player;
    Camera &_Camera;
    WindowRenderer &_Renderer;
    SoundManager &_SoundManager;

    Input MovementInputs[MOVEMENT_COUNT];
    Input ActionInputs[ACTIONS_COUNT];
    struct MouseInputs MouseInputs;

    const Uint8 *Keys;

    void HandleMouseInput();
    void HandleMovement();
    inline void PollAndUpdate(int actionIndex);
    void ValidatePlayerState();
    void ValidateCanMine();
    void UpdateEquippedAcceleration();

public:
    InputManager(World &world, Player *player,
                 Camera &camera,
                 WindowRenderer &renderer,
                 SoundManager &soundManager);
    ~InputManager();
    void Update(Uint64 tickCount);
    bool Running;
};
#endif
