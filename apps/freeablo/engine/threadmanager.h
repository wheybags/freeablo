#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <string>
#include <boost/lockfree/spsc_queue.hpp>

#include <audio/audio.h>

namespace FARender
{
    class RenderState;
}

namespace Engine
{
    enum ThreadState
    {
        musicPlay,
        renderState
    };

    struct Message
    {
        ThreadState type;

        union
        {
            std::string* musicPath;
            FARender::RenderState* renderState;
        } data;
    };

    class ThreadManager
    {
        public:
            static ThreadManager* get();

            ThreadManager();
            ~ThreadManager();

            void run();

            void playMusic(const std::string& path);
            void sendRenderState(FARender::RenderState* state);

        private:
            static ThreadManager* mThreadManager; ///< Singleton instance

            boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<100> > mQueue;
            void handleMessage(const Message& message);
            Audio::Music* mMusic;
            FARender::RenderState* mRenderState;
    };
}

#endif
