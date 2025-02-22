#define SDL_MAIN_HANDLED
#include "World.h"
#include "Player.h"
#include "WindowRenderer.h"
#include "InputManager.h"
#include <iostream>
#include <chrono>

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }
    World world(200, 200, 200, 30, 1000);
    Player player(&world);

    WindowRenderer renderer(&world, &player, 800, 600);
    InputManager inputManager(&world, &player, &renderer);

    renderer.Discover();
    world.tiles[(int)player.x][(int)player.y] = AIR;

    srand(SDL_GetTicks64());

    SDL_Event window_event;

    bool running = true;
    const Uint64 FRAME_DELAY = 16; //one frame every 16 ms ~= 60 fps
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
        renderer.ClearFrame();
        renderer.RenderFrame();
        inputManager.ManageInput();

        Uint64 frameTime = SDL_GetTicks64() - frameStart;
        if (frameTime < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    SDL_Quit();
}