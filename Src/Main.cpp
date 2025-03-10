#define SDL_MAIN_HANDLED
#include "Game.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        return 1;
    }

    World world(200, 200, 100, 100, 0);
    Player player(1.1f, 2.1f, 0.15f);

    EntityManager entityManager(world, player);
    WindowRenderer windowRenderer(world, player, 1280, 800);
    InputManager inputManager(world, player, windowRenderer);

    Uint64 frameRate = 1000 / 60;
    Game game(frameRate, world, entityManager, inputManager, windowRenderer);

    game.Start();

    SDL_Quit();
    return 0;
}