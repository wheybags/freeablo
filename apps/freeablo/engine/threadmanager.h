#pragma once
#include "../faaudio/audiomanager.h"
#include <string>

// clang-format off
#include <misc/disablewarn.h>
#include <rigtorp/SPSCQueue.h>
#include <misc/enablewarn.h>
// clang-format on

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
        RENDER_STATE,
        PRELOAD_SPRITES,
        CLEAR_SPRITES
    };

    struct Message
    {
        ThreadState type;

        union
        {
            std::string* musicPath;
            std::string* soundPath;
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
        bool isPlayingSound() const;
        void sendRenderState(FARender::RenderState* state);
        void sendSpritesForPreload(std::vector<uint32_t> sprites);
        void clearSprites();

    private:
        void handleMessage(const Message& message);

        static ThreadManager* mThreadManager; ///< Singleton instance
        rigtorp::SPSCQueue<Message> mQueue;
        FARender::RenderState* mRenderState;
        FAAudio::AudioManager mAudioManager;

        std::vector<uint32_t> mSpritesToPreload;
    };
}
