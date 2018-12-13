
#pragma once

// this header is included as a workaround for a bug in boost:
// https://svn.boost.org/trac10/ticket/13497
#include <boost/next_prior.hpp>

#include <boost/lockfree/spsc_queue.hpp>
#include <condition_variable>
#include <mutex>
#include <string>

#include "../faaudio/audiomanager.h"

namespace FARender
{
    class RenderState;
}

namespace Engine
{
    enum class ThreadState
    {
        PLAY_MUSIC,
        PLAY_SOUND,
        STOP_SOUND,
        PLAY_VIDEO,
        STOP_VIDEO,
        RENDER_STATE,
        PRELOAD_SPRITES
    };

    struct Message
    {
        ThreadState type;

        union
        {
            std::string* musicPath;
            std::string* soundPath;
            std::string* videoPath;
            FARender::RenderState* renderState;
            std::vector<uint32_t>* preloadSpriteIds;
        } data;
    };

    class ThreadManager
    {
    public:
        static ThreadManager* get();
        ThreadManager();
        void run();
        void playMusic(const std::string& path);
        void playSound(const std::string& path);
        void stopSound();
        void playVideo(const std::string& path);
        void stopVideo();
        void sendRenderState(FARender::RenderState* state);
        void sendSpritesForPreload(std::vector<uint32_t> sprites);
        bool waitForVideoComplete(int ms);
        bool videoInProgress() { return !waitForVideoComplete(0); }

    private:
        void handleMessage(const Message& message);

        static ThreadManager* mThreadManager; ///< Singleton instance
        boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<100>> mQueue;
        boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<10>> mvideoQueue;
        FARender::RenderState* mRenderState;
        FAAudio::AudioManager mAudioManager;

        std::vector<uint32_t> mSpritesToPreload;

        std::mutex mVideoPendingMutex;
        bool mVideoPending = false;
        std::condition_variable mVideoStartedCV;
    };
}
