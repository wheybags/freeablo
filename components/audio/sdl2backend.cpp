#include "audio.h"

#include <iostream>
#include <utility>
#include <stdint.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include <faio/fafileobject.h>


namespace Audio
{
    void init(size_t channelCount)
    {
        Mix_Init(0);
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;

        Mix_AllocateChannels(channelCount);
    }

    void quit()
    {
        Mix_CloseAudio();
        Mix_Quit();
    }

    Music* loadMusic(const std::string& path)
    {
        FAIO::FAFileObject f(path);
        size_t len = f.FAsize();
        uint8_t* buffer = new uint8_t[len];
        f.FAfread(buffer, 1, len);

        SDL_RWops *rw = SDL_RWFromMem(buffer, len);

        Music* mus = (Music*) new std::pair<Mix_Music*, uint8_t*>(Mix_LoadMUS_RW(rw, 1), buffer);
        return mus;
    }

    void freeMusic(Music* mus)
    {
        std::pair<Mix_Music*, uint8_t*>* data = (std::pair<Mix_Music*, uint8_t*>*)mus;
        Mix_FreeMusic(data->first);
        delete[] data->second;
        delete data;
    }

    void playMusic(Music* mus)
    {
        Mix_PlayMusic(((std::pair<Mix_Music*, uint8_t*>*)mus)->first, -1);
    }

    Sound* loadSound(const std::string& path)
    {
        FAIO::FAFileObject f(path);
        size_t len = f.FAsize();
        uint8_t* buffer = new uint8_t[len];
        f.FAfread(buffer, 1, len);

        SDL_RWops *rw = SDL_RWFromMem(buffer, len);

        Sound* sound = (Sound*) new std::pair<Mix_Chunk*, uint8_t*>(Mix_LoadWAV_RW(rw, 1), buffer);
        return sound;
    }

    void freeSound(Sound* sound)
    {
        std::pair<Mix_Chunk*, uint8_t*>* data = (std::pair<Mix_Chunk*, uint8_t*>*)sound;
        Mix_FreeChunk(data->first);
        delete[] data->second;
        delete data;
    }

    int32_t playSound(Sound* sound)
    {
        std::pair<Mix_Chunk*, uint8_t*>* data = (std::pair<Mix_Chunk*, uint8_t*>*)sound;
        return Mix_PlayChannel(-1, data->first, 0);
    }

    void stopSound()
    {
        Mix_HaltChannel(-1);
    }

    bool channelPlaying(int32_t channel)
    {
        return Mix_Playing(channel) != 0;
    }
}
