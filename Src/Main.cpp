#define SDL_MAIN_HANDLED
#include "Game.h"
#include <iostream>

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }

    Game* game = new Game(1000/60);

    game->Start();

    delete game;
    
    SDL_Quit();
}