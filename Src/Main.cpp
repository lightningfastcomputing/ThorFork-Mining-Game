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
        Vec2 worldDim = {100, 300};
        Uint64 frameRate = 1000 / 60;

        game = new Game(frameRate, worldDim);
#ifdef WASM
        emscripten_set_main_loop_arg(emscripten_loop, nullptr, 0, 1);
#else
        game->Start();
#endif
    }

    TTF_Quit();
    IMG_Quit();

    SDL_Quit();

    return 0;
}