#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <string>
#include <boost/lockfree/spsc_queue.hpp>

#include "../faaudio/audiomanager.h"

namespace FARender
{
    class RenderState;
}

namespace Engine
{
    enum ThreadState
    {
        PLAY_MUSIC,
        PLAY_SOUND,
        STOP_SOUND,
        RENDER_STATE
    };

    struct Message
    {
        ThreadState type;

        union
        {
            std::string* musicPath;
            std::string* soundPath;
            FARender::RenderState* renderState;
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
            void sendRenderState(FARender::RenderState* state);

        private:
            void handleMessage(const Message& message);

            static ThreadManager* mThreadManager; ///< Singleton instance
            boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<100> > mQueue;
            FARender::RenderState* mRenderState;
            FAAudio::AudioManager mAudioManager;
    };
}

#endif
