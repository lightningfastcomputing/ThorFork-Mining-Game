#define SDL_MAIN_HANDLED
#include "Game.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }

    World world(200, 200, 100, 100, 0);
    Player player(1.5f, 2.5f, 0.15f);

    EntityManager entityManager(world, player);
    WindowRenderer windowRenderer(world, player, 1280, 800);
    InputManager inputManager(world, player, windowRenderer);

    Uint64 frameRate = 1000 / 60;
    Game game(frameRate, world, entityManager, inputManager, windowRenderer);

    game.Start();

    SDL_Quit();
}