#include "InputManager.h"

// dst = sqrt((x-x0)^2 + (y-y0^2))
float Distance(int x0, int y0, int x1, int y1)
{
    float distance = SDL_sqrtf(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0)));
    // printf("DISTANCE: %f\n", distance);
    return distance;
}

InputManager::InputManager(World &world, Player &player, WindowRenderer &renderer) : _World(world), _Player(player), _Renderer(renderer)
{
    Keys = SDL_GetKeyboardState(NULL);
    Running = true;
}

InputManager::~InputManager()
{
}

void InputManager::ManageInput()
{
    SDL_PumpEvents();
    int mouseState = SDL_GetMouseState(&(_Renderer.MouseX), &(_Renderer.MouseY));
    int selectedX = _Renderer.MouseWorldX, selectedY = _Renderer.MouseWorldY;
    if (SDL_BUTTON(mouseState) == 1 && _Player.CanMine && _Renderer.IsDiscovered(selectedX, selectedY))
    {
        _Player.Score += _World.DestroyTile(selectedX, selectedY);
    }
    direction dir = NONE;
    if (Keys[SDL_SCANCODE_W] && !(Keys[SDL_SCANCODE_A] || Keys[SDL_SCANCODE_D]))
        dir = NORTH;
    else if (Keys[SDL_SCANCODE_S] && !(Keys[SDL_SCANCODE_A] || Keys[SDL_SCANCODE_D]))
        dir = SOUTH;
    else if (Keys[SDL_SCANCODE_A] && !(Keys[SDL_SCANCODE_W] || Keys[SDL_SCANCODE_S]))
        dir = WEST;
    else if (Keys[SDL_SCANCODE_D] && !(Keys[SDL_SCANCODE_W] || Keys[SDL_SCANCODE_S]))
        dir = EAST;
    else if (Keys[SDL_SCANCODE_W] && Keys[SDL_SCANCODE_A])
        dir = NORTHWEST;
    else if (Keys[SDL_SCANCODE_W] && Keys[SDL_SCANCODE_D])
        dir = NORTHEAST;
    if (Keys[SDL_SCANCODE_S] && Keys[SDL_SCANCODE_A])
        dir = SOUTHWEST;
    else if (Keys[SDL_SCANCODE_S] && Keys[SDL_SCANCODE_D])
        dir = SOUTHEAST;
    _Player.UpdateVelocity(dir);
    if (Keys[SDL_SCANCODE_TAB])
        _Renderer.Reveal();
    if (Keys[SDL_SCANCODE_F1])
        _Renderer.ToggleDebug();
    if (Keys[SDL_SCANCODE_ESCAPE])
        Running = false;

    if (Distance(_Player.x, _Player.y, _Renderer.MouseWorldX, _Renderer.MouseWorldY) <= _Player.MiningRadius)
        _Player.CanMine = true;
    else
        _Player.CanMine = false;
}
