#include "Game.h"

Game::Game(Uint64 frameRate, World &world, EntityManager &entityManager, InputManager &inputManager, WindowRenderer &windowRenderer, SoundManager &soundManager)
    : FrameRate(frameRate), _World(world), _EntityManager(entityManager), _InputManager(inputManager), _WindowRenderer(windowRenderer), _SoundManager(soundManager)
{
    Running = true;
}
Game::~Game()
{
}

void Game::Start()
{
    SDL_Event event;

    Running = this->Running && _InputManager.Running && _WindowRenderer.Running;
    while (Running)
    {
        Uint64 frameStart = SDL_GetTicks64();
        TickCount++;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                Running = false;
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    _WindowRenderer.UpdateWindow();
                    break;
                }
            }
        }

        _WindowRenderer.Update(TickCount);
        _InputManager.Update(TickCount);
        _World.Update(TickCount);
        _EntityManager.Update();

        Uint64 frameTime = SDL_GetTicks64() - frameStart;
        if (frameTime < FrameRate)
        {
            SDL_Delay(FrameRate - frameTime);
        }
        else {
            //printf("Frame took longer to run than Framerate: %llu\n", frameTime - FrameRate);
        }
        Running = this->Running && _InputManager.Running && _WindowRenderer.Running;
        TickCount++;
    }
}
