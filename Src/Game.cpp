#include "Game.h"

Game::Game(Uint64 frameRate, World &world, Player &player, InputManager &inputManager, WindowRenderer &windowRenderer)
    : _World(world), _Player(player), _WindowRenderer(windowRenderer), _InputManager(inputManager), FrameRate(frameRate)
{
    _World.tiles[(int)_Player.x][(int)_Player.y] = AIR;
    _WindowRenderer.Discover();

    srand(SDL_GetTicks64());
}
Game::~Game()
{
}
void Game::Start()
{
    SDL_Event window_event;

    bool running = _InputManager.Running && _WindowRenderer.Running;
    while (running)
    {
        Uint64 frameStart = SDL_GetTicks64();

        if (SDL_PollEvent(&window_event))
        {
            if (window_event.type == SDL_QUIT)
            {
                running = false;
            }
        }
        _WindowRenderer.ClearFrame();
        _WindowRenderer.RenderFrame();
        _InputManager.ManageInput();

        Uint64 frameTime = SDL_GetTicks64() - frameStart;
        if (frameTime < this->FrameRate)
        {
            SDL_Delay(this->FrameRate - frameTime);
        }
        // running = false;
        running = _InputManager.Running && _WindowRenderer.Running;
    }
}
