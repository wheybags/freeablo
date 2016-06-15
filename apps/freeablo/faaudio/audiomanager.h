#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <audio/audio.h>

namespace Engine
{
    class ThreadManager;
}

namespace FAAudio
{
    struct CacheEntry
    {
        Audio::Sound* sound;
        std::list<std::string>::iterator usedListIt;

        CacheEntry(Audio::Sound* _sound, std::list<std::string>::iterator _usedListIt)
            : sound(_sound), usedListIt(_usedListIt) {}

        CacheEntry() {}
    };

    class AudioManager
    {
        public:
            AudioManager(int32_t channelCount, size_t cacheSize);
            ~AudioManager();

            void playSound(const std::string& path);
            void stopSound();
            void playMusic(const std::string& path);

        private:
            std::vector<std::string> mPlaying;
            std::map<std::string, CacheEntry> mCache;
            std::list<std::string> mUsedList;
            size_t mCacheSize;
            size_t mCount;
            Audio::Music* mCurrentMusic;
    };
}

#endif
