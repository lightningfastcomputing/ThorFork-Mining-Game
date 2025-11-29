#include "Game.h"

Game::Game(Uint64 frameRate, Vec2 worldDimensions)
{
    _SoundManager = new SoundManager();

    _World = new World(worldDimensions.x, worldDimensions.y, *_SoundManager);

    Player *player = new Player(1.5f, 1.5f, 0.5f);
    _World->AddPlayer(player);

    Vec2 screenDim = {1280, 800};
    Camera* camera = new Camera(player, screenDim);

    _WindowRenderer = new WindowRenderer(*_World, *camera, screenDim.x, screenDim.y);
    _InputManager = new InputManager(*_World, player, *camera, *_WindowRenderer, *_SoundManager);
    FrameRate = frameRate;

    Running = true;
}

Game::~Game()
{
}

void Game::Start()
{
    Running = this->Running && _InputManager->Running && _WindowRenderer->Running && _SoundManager->Running;
    while (Running)
    {
        Update();
    }
}

void Game::Update()
{
    Uint64 frameStart = SDL_GetTicks64();
    TickCount++;

    while (SDL_PollEvent(&Event))
    {
        if (Event.type == SDL_QUIT)
        {
            Running = false;
        }
        if (Event.type == SDL_WINDOWEVENT)
        {
            switch (Event.window.event)
            {
            case SDL_WINDOWEVENT_RESIZED:
                _WindowRenderer->UpdateWindow();
                break;
            }
        }
    }
    
    _InputManager->Update(TickCount);
    _World->Update(TickCount);
    _WindowRenderer->Update(TickCount);

    Running = this->Running && _InputManager->Running && _WindowRenderer->Running && _SoundManager->Running;
    

    Uint64 frameTime = SDL_GetTicks64() - frameStart;
    if (frameTime < FrameRate)
    {
        SDL_Delay(FrameRate - frameTime);
    }
    else
    {
        // printf("Frame took longer to run than Framerate: %llu\n", frameTime - FrameRate);
    }
}
