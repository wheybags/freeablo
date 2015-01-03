#include "audio.h"

#include <iostream>
#include <utility>
#include <stdint.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include <faio/faio.h>


namespace Audio
{
    void init()
    {
        if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    Music* loadMusic(const std::string& path)
    {
        FAIO::FAFile* f = FAIO::FAfopen(path);
        size_t len = FAIO::FAsize(f);
        uint8_t* buffer = (uint8_t*)malloc(len);
        FAIO::FAfread(buffer, 1, len, f);
        FAIO::FAfclose(f);

        SDL_RWops *rw = SDL_RWFromMem(buffer, len); 

        Music* mus = (Music*) new std::pair<Mix_Music*, uint8_t*>(Mix_LoadMUS_RW(rw, 0), buffer);
        return mus;
    }

    void freeMusic(Music* mus)
    {
        std::pair<Mix_Music*, uint8_t*>* data = (std::pair<Mix_Music*, uint8_t*>*)mus;
        Mix_FreeMusic(data->first);
        delete data->second; // workaround for the freesrc param of Mix_LoadMus in loadMusic above apparently just not working
        delete data;
    }

    void playMusic(Music* mus)
    {
        Mix_PlayMusic(((std::pair<Mix_Music*, uint8_t*>*)mus)->first, -1);
    }
}
