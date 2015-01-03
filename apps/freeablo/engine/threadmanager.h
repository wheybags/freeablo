#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <string>
#include <boost/atomic.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <Rocket/Core.h>

#include <level/level.h>
#include <audio/audio.h>

namespace Engine
{
    enum ThreadState
    {
        guiLoadTexture,
        guiGenerateTexture,
        guiReleaseTexture,
        running,
        levelChange,
        stopped,
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

            bool loadGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
            bool generateGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
            void releaseGuiTextureFunc(Rocket::Core::TextureHandle texture_handle);
            void setLevel(const Level::Level* level);

        private:
            static ThreadManager* mThreadManager; ///< Singleton instance

            boost::atomic<ThreadState> mThreadState;
            void* mThreadCommunicationTmp;

            boost::lockfree::spsc_queue<Message, boost::lockfree::capacity<100> > mQueue;
            void handleMessage(const Message& message);
            Audio::Music* mMusic;
    };
}

#endif
