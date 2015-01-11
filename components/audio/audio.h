#ifndef AUDIO_H
#define AUDIO_H

#include <string>

namespace Audio
{
    void init();
    void quit();
    
    typedef void Music;
    Music* loadMusic(const std::string& path);
    void freeMusic(Music* mus);
    void playMusic(Music* mus);
}

#endif
