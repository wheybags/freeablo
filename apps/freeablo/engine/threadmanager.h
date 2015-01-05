#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <string>
#include <boost/lockfree/spsc_queue.hpp>

#include <audio/audio.h>

namespace Engine
{
    enum ThreadState
    {
        musicPlay
    };

    struct Message
    {
        ThreadState type;

        union
        {
            std::string* musicPath;
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

        private:
            static ThreadManager* mThreadManager; ///< Singleton instance

            boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<100> > mQueue;
            void handleMessage(const Message& message);
            Audio::Music* mMusic;
    };
}

#endif
