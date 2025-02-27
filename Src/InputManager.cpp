#include "InputManager.h"

enum Movement
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum Action
{
    REVEAL,
    TOGGLE_DEBUG,
    EXIT
};

InputManager::InputManager(World &world, Player &player, WindowRenderer &renderer) : _World(world),
                                                                                     _Player(player), _Renderer(renderer)
{
    Keys = SDL_GetKeyboardState(NULL);
    Running = true;

    MovementInputs[UP] = {SDL_SCANCODE_W, 0, 0, nullptr};
    MovementInputs[DOWN] = {SDL_SCANCODE_S, 0, 0, nullptr};
    MovementInputs[LEFT] = {SDL_SCANCODE_A, 0, 0, nullptr};
    MovementInputs[RIGHT] = {SDL_SCANCODE_D, 0, 0, nullptr};

    MouseInputs = {500, 0, 500, 0};

    ActionInputs[REVEAL] = {SDL_SCANCODE_TAB, 500, 0, [this]()
                            { Reveal(); }};
    ActionInputs[TOGGLE_DEBUG] = {SDL_SCANCODE_F1, 500, 0, [this]()
                                  { ToggleDebug(); }};
    ActionInputs[EXIT] = {SDL_SCANCODE_ESCAPE, 100, 0, [this]()
                          { Exit(); }};
}

InputManager::~InputManager()
{
}

void InputManager::ManageInput()
{
    SDL_PumpEvents();

    HandleMouseInput();

    HandleMovement();

    PollAndUpdate(REVEAL);
    PollAndUpdate(TOGGLE_DEBUG);
    PollAndUpdate(EXIT);

    UpdatePlayer();
}

void InputManager::HandleMouseInput()
{
    int mouseState = SDL_GetMouseState(&(_Renderer.MouseX), &(_Renderer.MouseY));
    int selectedX = _Renderer.MouseWorldX, selectedY = _Renderer.MouseWorldY;
    Uint64 now = SDL_GetTicks64();

    if (mouseState & SDL_BUTTON(1) && (now - MouseInputs.LeftLastTimePressed) > MouseInputs.LeftCooldown)
    {
        if (_Player.CanMine && _Renderer.IsDiscovered(selectedX, selectedY))
        {
            _World.DestroyTile(selectedX, selectedY);
            MouseInputs.LeftLastTimePressed = now;
        }
    }
    else if (mouseState & SDL_BUTTON(3) && (now - MouseInputs.RightLastTimePressed) > MouseInputs.RightCooldown)
    {
        if (_Player.CanMine && _Renderer.IsDiscovered(selectedX, selectedY))
        {
            _World.ChangeTile(selectedX, selectedY, EXPLOSIVE);
            MouseInputs.RightLastTimePressed = now;
        }
    }
}

void InputManager::HandleMovement()
{
    SDL_Scancode up = MovementInputs[UP].ScanCode, down = MovementInputs[DOWN].ScanCode,
                 left = MovementInputs[LEFT].ScanCode, right = MovementInputs[RIGHT].ScanCode;

    bool horizontalInput = Keys[left] || Keys[right];
    bool verticalInput = Keys[up] || Keys[down];

    direction dir = NONE;
    if (Keys[up] && !horizontalInput)
        dir = NORTH;
    else if (Keys[down] && !horizontalInput)
        dir = SOUTH;
    else if (Keys[left] && !verticalInput)
        dir = WEST;
    else if (Keys[right] && !verticalInput)
        dir = EAST;
    else if (Keys[up] && Keys[left])
        dir = NORTHWEST;
    else if (Keys[up] && Keys[right])
        dir = NORTHEAST;
    else if (Keys[down] && Keys[left])
        dir = SOUTHWEST;
    else if (Keys[down] && Keys[right])
        dir = SOUTHEAST;
    _Player.UpdateVelocity(dir);
}

void InputManager::PollAndUpdate(int actionIndex)
{
    Input &input = ActionInputs[actionIndex];
    Uint64 now = SDL_GetTicks64();
    if (Keys[input.ScanCode] && (now - input.LastTimePressed) >= input.MillisecondCooldown)
    {
        input.Action();
        input.LastTimePressed = now;
    }
}

void InputManager::UpdatePlayer()
{
    float mouseWorldX = _Renderer.MouseWorldX, mouseWorldY = _Renderer.MouseWorldY;

    float adjustedX = _Player.x + _Player.HalfSize, adjustedY = _Player.y + _Player.HalfSize;
    float distance = Utils::Distance(_Player.x, _Player.y,  mouseWorldX, mouseWorldY);

    bool selectedIsInsidePlayer = (mouseWorldX >= _Player.xStart && mouseWorldX <= _Player.xEnd) &&
                        (mouseWorldY >= _Player.yStart && mouseWorldY <= _Player.yEnd);  

    _Player.CanMine = (distance <= _Player.MiningRadius) && !selectedIsInsidePlayer;
}

void InputManager::Reveal() { this->_Renderer.Reveal(); }
void InputManager::ToggleDebug() { this->_Renderer.ToggleDebug(); }
void InputManager::Exit() { this->Running = false; }
