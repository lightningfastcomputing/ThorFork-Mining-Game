#include "Game.h"

Game::Game(Uint64 frameRate, World &world, EntityManager &entityManager, InputManager &inputManager, WindowRenderer &windowRenderer)
    : FrameRate(frameRate), _World(world), _EntityManager(entityManager), _InputManager(inputManager), _WindowRenderer(windowRenderer)
{
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
        _EntityManager.UpdatePlayerPosition();

        Uint64 frameTime = SDL_GetTicks64() - frameStart;
        if (frameTime < this->FrameRate)
        {
            SDL_Delay(this->FrameRate - frameTime);
        }
        // running = false;
        running = _InputManager.Running && _WindowRenderer.Running;
    }
}
