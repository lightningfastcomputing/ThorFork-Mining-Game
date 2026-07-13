#include "SoundManager.h"

void Callback(void *userdata, Uint8 *stream, int len)
{
    UserData *data = (UserData *)userdata;
    SDL_memset(stream, 0, len);
    int16_t *out = (int16_t *)stream;

    for (int i = 0; i < CHANNELS; i++)
    {
        WAVData *channel = &data->Channels[i];
        if (channel->len <= 0)
            continue;

        int bytesToMix = (channel->len < (Uint32)len) ? channel->len : len;
        int samplesToMix = bytesToMix / sizeof(int16_t);
        int16_t *src = (int16_t *)channel->pos;

        for (int j = 0; j < samplesToMix; j++)
        {
            int mixed = out[j] + (int)(src[j]);
            out[j] = Utils::clamp16(mixed);
        }

        channel->pos += bytesToMix;
        channel->len -= bytesToMix;
    }
}

SoundManager::SoundManager()
{
    Running = true;

    SDL_zero(UserData);

    for (int i = 0; i < SOUNDS_COUNT; i++)
        WAVs[i] = nullptr;

    Mutex = SDL_CreateMutex();

    if (!Mutex)
    {
        printf("Failed to create audio mutex: %s\n", SDL_GetError());
        Running = false;
        return;
    }

    SDL_zero(DeviceSpec);
    DeviceSpec.freq = 44100;
    DeviceSpec.format = AUDIO_S16SYS;
    DeviceSpec.channels = 2;
    DeviceSpec.samples = 1024;
    DeviceSpec.userdata = &UserData;
    DeviceSpec.callback = Callback;

    if (SDL_OpenAudio(&DeviceSpec, nullptr) < 0)
    {
        printf("Failed to open audio: %s\n", SDL_GetError());
        Running = false;
        return;
    }

    WAVs[FOOTSTEP] = LoadAndConvertWAVFromFile("Sounds/Footstep.wav");
    WAVs[PICKAXE_STRIKE] = LoadAndConvertWAVFromFile("Sounds/PickaxeStrike.wav");
    WAVs[EXPLOSION] = LoadAndConvertWAVFromFile("Sounds/Explosion.wav");
    WAVs[ROCK_CRUMBLE] = LoadAndConvertWAVFromFile("Sounds/RockCrumble.wav");

    SDL_PauseAudio(0);
}

SoundManager::~SoundManager()
{
    SDL_CloseAudio();
    for (int i = 0; i < SOUNDS_COUNT; i++)
    {
        delete WAVs[i]->pos;
        delete WAVs[i];
    }
}

WAVData *SoundManager::LoadAndConvertWAVFromFile(const char *file)
{
    SDL_AudioSpec spec;
    Uint8 *wavBuffer = nullptr;
    Uint32 wavLength;

    if (SDL_LoadWAV(file, &spec, &wavBuffer, &wavLength) == NULL)
    {
        printf("Failed to load WAV from %s: %s\n", file, SDL_GetError());
        goto fail;
    }
    else
    {
        SDL_AudioCVT cvt;
        int result = SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
                                       DeviceSpec.format, DeviceSpec.channels, DeviceSpec.freq);

        if (result < 0)
        {
            printf("Failed to build CVT: %s\n", SDL_GetError());
            goto fail;
        }
        else if (result == 1)
        {
            cvt.len = wavLength;
            cvt.buf = new Uint8[cvt.len * cvt.len_mult];

            SDL_memcpy(cvt.buf, wavBuffer, wavLength);

            if (SDL_ConvertAudio(&cvt) < 0)
            {
                printf("Failed to convert audio: %s\n", SDL_GetError());
                delete cvt.buf;
                goto fail;
            }
            return new WAVData(cvt.buf, cvt.len_cvt);
        }
        else
        {
            return new WAVData(wavBuffer, wavLength);
        }
    }
fail:
    SDL_FreeWAV(wavBuffer);
    Running = false;
    return nullptr;
}

void SoundManager::PlaySound(Sound idx)
{
    if (idx < 0 || idx >= SOUNDS_COUNT)
        return;

    WAVData *data = WAVs[idx];

    if (!data || !data->pos || data->len == 0)
        return;

    SDL_LockAudio();

    for (int i = 0; i < CHANNELS; i++)
    {
        WAVData *emptyChannel = &UserData.Channels[i];

        if (emptyChannel->len <= 0)
        {
            emptyChannel->len = data->len;
            emptyChannel->pos = data->pos;
            break;
        }
    }

    SDL_UnlockAudio();
}