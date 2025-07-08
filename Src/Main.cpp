#define SDL_MAIN_HANDLED
#include "Game.h"
//#include <SDL2/SDL_net.h>

int main(int argc, char *argv[])
{

    ValueNoise2D noise(1234); // Seeded for repeatability

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        return 1;
    }

    SoundManager soundManager;

    int worldWidth = 50;
    int worldHeight = 50;

    unsigned long size = sizeof(TileState);

    World world(worldWidth, worldHeight);

    Player player(1.1f, 2.1f, 0.15f, worldWidth, worldHeight);

    std::vector<Player*> players;
    players.push_back(&player);

    EntityManager entityManager(world, players);
    WindowRenderer windowRenderer(world, &player, players, 1280, 800);
    InputManager inputManager(world, &player, players, windowRenderer, soundManager);
    Uint64 frameRate = 1000 / 60;
    Game game(frameRate, world, entityManager, inputManager, windowRenderer, soundManager);

    game.Start();

    SDL_Quit();
    return 0;
}