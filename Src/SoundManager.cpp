#include "SoundManager.h"

SoundManager::SoundManager()
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(64);
    Mix_ReserveChannels(4);

    Sounds[PICKAXE_STRIKE] = Mix_LoadWAV("Sounds/PickaxeStrike.wav");
    Sounds[FOOTSTEP] = Mix_LoadWAV("Sounds/Footstep.wav");

    for (int i = 0; i < SOUNDS_COUNT; i++)
    {
        if (!Sounds[i])
        {
            std::cout << "Error Loading Sounds: " << Mix_GetError() << std::endl;
        }
    }
}

SoundManager::~SoundManager()
{
    for (int i = 0; i < SOUNDS_COUNT; i++)
    {
        if (Sounds[i] != nullptr)
        {
            Mix_FreeChunk(Sounds[i]);
        }
    }
    Mix_CloseAudio();
}

void SoundManager::PlaySound(Sound idx)
{
    Mix_PlayChannel(-1, Sounds[idx], 0);
}