
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
#include <queue>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "World.h"

enum Sound
{
    PICKAXE_STRIKE,
    DYNAMITE_EXPLOSION,
    FOOTSTEP,
    SOUNDS_COUNT
};

class SoundManager
{
private:
    Mix_Chunk *Sounds[SOUNDS_COUNT];

public:
    SoundManager();
    ~SoundManager();

    void PlaySound(Sound idx);
};

#endif