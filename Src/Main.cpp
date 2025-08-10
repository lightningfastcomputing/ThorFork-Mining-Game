#define SDL_MAIN_HANDLED

#ifdef WASM
#include <emscripten.h>
#endif

#include "Game.h"
//#include <SDL2/SDL_net.h>
Game* game;

#ifdef WASM
    void emscripten_loop(void* arg)
    {
        game->Update();
    }
#endif

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

        Player* player = new Player(1.8f, 1.8f, 0.5f);

        std::vector<Entity*> entities;
        entities.push_back(player);

        EntityManager entityManager(world, entities);
        WindowRenderer windowRenderer(world, player, entities, 1280, 800);
        InputManager inputManager(world, player, windowRenderer, soundManager, entityManager);
        Uint64 frameRate = 1000 / 60;
        game = new Game(frameRate, world, entityManager, inputManager, windowRenderer, soundManager);
#ifdef WASM
        emscripten_set_main_loop_arg(emscripten_loop, nullptr, 0, 1);
#else
        game->Start();
#endif
    }

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();

    SDL_Quit();

    return 0;
}