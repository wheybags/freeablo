#pragma once
#include "../faworld/world.h"
#include "renderer.h"

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FARender
{
    class AnimationPlayer
    {
    public:
        enum class AnimationType : uint8_t
        {
            Looped,
            Once,
            FreezeAtEnd,
            BySequence,

            ENUM_END
        };

        AnimationPlayer() {}
        AnimationPlayer(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        std::pair<FARender::FASpriteGroup*, int32_t> getCurrentFrame();
        AnimationType getCurrentAnimationType() { return mPlayingAnimType; }

        void playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick frameDuration, AnimationType type, int32_t startFrame = 0);
        void playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick frameDuration, std::vector<int32_t> frameSequence);

        void stopAnimation();
        bool isPlaying() { return mCurrentAnim != nullptr; }

        //!
        //! Simply replaces the currently running animation.
        //! The difference from playAnimation is that it will not start at the start of the new animation,
        //! but at whatever point the animation running before it was at, ie it "replaces" instead of
        //! playing a new animation
        void replaceAnimation(FARender::FASpriteGroup* anim);

        void update();
        int32_t getAnimLength() const;
        struct nk_image getCurrentNkImage();

    private:
        FARender::FASpriteGroup* mCurrentAnim = nullptr;

        FAWorld::Tick mPlayingAnimDuration = 0;
        AnimationType mPlayingAnimType = AnimationType::Once;
        FAWorld::Tick mTicksSinceAnimStarted = 0;
        std::vector<int32_t> mFrameSequence;
    };
}
