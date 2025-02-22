#include "Game.h"

Game::Game(Uint64 FrameRate)
{
    _World = new World(200, 200, 200, 30, 1000);
    _Player = new Player(_World);
    _WindowRenderer = new WindowRenderer(_World, _Player, 800, 600);
    _InputManager = new InputManager(_World, _Player, _WindowRenderer);

    this->FrameRate = FrameRate;

    _World->tiles[(int)_Player->x][(int)_Player->y] = AIR;
    _WindowRenderer->Discover();

    srand(SDL_GetTicks64());
}
Game::~Game()
{
}
void Game::Start()
{
    SDL_Event window_event;

    bool running = _InputManager->Running && _WindowRenderer->Running;
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
        _WindowRenderer->ClearFrame();
        _WindowRenderer->RenderFrame();
        _InputManager->ManageInput();

        Uint64 frameTime = SDL_GetTicks64() - frameStart;
        if (frameTime < this->FrameRate)
        {
            SDL_Delay(this->FrameRate - frameTime);
        }
        //running = false;
        running = _InputManager->Running && _WindowRenderer->Running;
    }
}
