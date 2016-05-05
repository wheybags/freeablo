#include "audiomanager.h"
#include <assert.h>
#include <iostream>
#include <string>

namespace FAAudio
{
    AudioManager::AudioManager(int32_t channelCount, size_t cacheSize) : mCacheSize(cacheSize), mCount(0), mCurrentMusic(NULL)
    {
        Audio::init(channelCount);
        mPlaying.resize(channelCount);
    }

    AudioManager::~AudioManager()
    {
        for(std::map<std::string, CacheEntry>::iterator it = mCache.begin(); it != mCache.end(); ++it)
            Audio::freeSound(it->second.sound);

        if(mCurrentMusic)
            Audio::freeMusic(mCurrentMusic);

        Audio::quit();
    }

    void AudioManager::playSound(const std::string& path)
    {
        if(mCache.find(path) == mCache.end())
        {
            if(mCount >= mCacheSize)
            {
                // find the least recently used CacheEntry that is not still playing, and evict it
                std::list<std::string>::reverse_iterator it = mUsedList.rbegin();
                for(; it != mUsedList.rend(); ++it)
                {
                    bool playing = false;

                    for(size_t i = 0; i < mPlaying.size(); i++)
                    {
                        if(mPlaying[i] == *it)
                        {
                            if(Audio::channelPlaying(i))
                            {
                                playing = true;
                                break;
                            }
                        }
                    }

                    if(!playing)
                        break;
                }

                assert(it != mUsedList.rend() && "no evictable sounds found, this should never happen");

                std::cerr << "EVICTING " << *it << std::endl;

                CacheEntry toEvict = mCache[*it];

                Audio::freeMusic(toEvict.sound);

                mCache.erase(*it);
                mUsedList.erase(--(it.base()));
                mCount--;
            }

            mUsedList.push_front(path);
            mCache[path] = CacheEntry(Audio::loadSound(path), mUsedList.begin());
            mCount++;
        }
        else
        {
            // move to top of used list
            mUsedList.erase(mCache[path].usedListIt);
            mUsedList.push_front(path);
            mCache[path].usedListIt = mUsedList.begin();
        }

        int channel = Audio::playSound(mCache[path].sound);
        if(channel >= 0)
            mPlaying[channel] = path;
    }

    void AudioManager::stopSound()
    {
        Audio::stopSound();
    }

    void AudioManager::playMusic(const std::string& path)
    {
        if(mCurrentMusic != NULL)
            Audio::freeMusic(mCurrentMusic);

        mCurrentMusic = Audio::loadMusic(path);
        Audio::playMusic(mCurrentMusic);
    }
}
