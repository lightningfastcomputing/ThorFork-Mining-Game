#define SDL_MAIN_HANDLED
#include "Game.h"
//#include <SDL2/SDL_net.h>

int main(int argc, char *argv[])
{

    {
        if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
        {
            fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
            return 1;
        }

        SoundManager soundManager;

        int worldWidth = 400;
        int worldHeight = 300;


        World world(worldWidth, worldHeight);

        Player* player = new Player(1.8f, 2.8f, 0.15f);

        std::vector<Entity*> entities;
        entities.push_back(player);

        EntityManager entityManager(world, entities);
        WindowRenderer windowRenderer(world, player, entities, 1280, 800);
        InputManager inputManager(world, player, windowRenderer, soundManager, entityManager);
        Uint64 frameRate = 1000 / 60;
        Game game(frameRate, world, entityManager, inputManager, windowRenderer, soundManager);

        game.Start();
    }

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();

    SDL_Quit();

    return 0;
}