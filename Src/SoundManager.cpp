#include "SoundManager.h"

SoundManager::SoundManager()
{
    int freq;
    Uint16 format;
    int channels;

    Mix_QuerySpec(&freq, &format, &channels);
    printf("Frequency: %d, Format: %d, Channels: %d\n", freq, format, channels);

    Mix_OpenAudio(freq, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(256);
    Mix_ReserveChannels(4);

    Sounds[PICKAXE_STRIKE] = Mix_LoadWAV("Sounds/PickaxeStrike.wav");

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
        if (Sounds[i])
        {
            Mix_FreeChunk(Sounds[i]);
        }
    }
    Mix_CloseAudio();
}

void SoundManager::PlaySound(Sound idx)
{
    Mix_PlayChannelTimed(1, Sounds[idx], 0, 500);
    Mix_ExpireChannel(1, 500);
}