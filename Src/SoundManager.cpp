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
    if (SDL_GetAudioDeviceSpec(1, 0, &DeviceSpec) != 0)
        goto error;

    DeviceSpec.samples = 1024;

    WAVs[FOOTSTEP] = LoadAndConvertWAVFromFile("Sounds/Footstep.wav");
    WAVs[PICKAXE_STRIKE] = LoadAndConvertWAVFromFile("Sounds/PickaxeStrike.wav");
    WAVs[EXPLOSION] = LoadAndConvertWAVFromFile("Sounds/Explosion.wav");

    DeviceSpec.userdata = &UserData;
    DeviceSpec.callback = Callback;

    if (SDL_OpenAudio(&DeviceSpec, nullptr) < 0)
        goto error;
    

    SDL_PauseAudio(0);

    return;
error:
    Running = false;
}

SoundManager::~SoundManager()
{
    SDL_CloseAudioDevice(1);
    for (int i = 0; i < SOUNDS_COUNT; i++)
    {
        delete WAVs[i]->pos;
        delete WAVs[i];
    }
}

WAVData *SoundManager::LoadAndConvertWAVFromFile(const char *file)
{
    SDL_AudioSpec spec;
    Uint8 *wavBuffer;
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
                                       DeviceSpec.format, DeviceSpec.channels, DeviceSpec.freq) < 0;

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
    WAVData *data = WAVs[idx];

    for (int i = 0; i < CHANNELS; i++)
    {
        WAVData* emptyChannel = &UserData.Channels[i];
        if (emptyChannel->len <= 0)
        {
            emptyChannel->len = data->len;
            emptyChannel->pos = data->pos;
            break;
        }
    }
}