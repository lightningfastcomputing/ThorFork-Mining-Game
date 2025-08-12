
#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H
#include <queue>
#include <iostream>
#include <SDL2/SDL.h>
#include "Utils.h"

#define CHANNELS 16

enum Sound
{
    PICKAXE_STRIKE,
    FOOTSTEP,
    EXPLOSION,
    SOUNDS_COUNT
};

struct WAVData
{
    Uint8 *pos;
    Uint32 len;
};

struct UserData
{
    WAVData Channels[CHANNELS];
    int ActiveCount;
    bool Underground;
};

class SoundManager
{
private:
    WAVData *WAVs[SOUNDS_COUNT] = {nullptr};
    UserData UserData;

    WAVData *LoadAndConvertWAVFromFile(const char *file);
    SDL_AudioSpec DeviceSpec;

public:
    SoundManager();
    ~SoundManager();
    bool Running;

    void PlaySound(Sound idx);
};

#endif