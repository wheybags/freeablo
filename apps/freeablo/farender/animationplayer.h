#pragma once
#include "../faworld/world.h"
#include "renderer.h"
#include <fa_nuklear.h>

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

        AnimationPlayer() = default;
        void load(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        std::pair<Render::SpriteGroup*, int32_t> getCurrentFrame() const;
        AnimationType getCurrentAnimationType() { return mPlayingAnimType; }

        void playAnimation(Render::SpriteGroup* anim, FAWorld::Tick frameDuration, AnimationType type, int32_t startFrame = 0);
        void playAnimation(Render::SpriteGroup* anim, FAWorld::Tick frameDuration, std::vector<int32_t> frameSequence);

        void stopAnimation();
        bool isPlaying() { return mCurrentAnim != nullptr; }

        //!
        //! Simply replaces the currently running animation.
        //! The difference from playAnimation is that it will not start at the start of the new animation,
        //! but at whatever point the animation running before it was at, ie it "replaces" instead of
        //! playing a new animation
        void replaceAnimation(Render::SpriteGroup* anim);

        void update();
        int32_t getAnimLength() const;
        struct nk_image getCurrentNkImage();

        // This is not used for the game simulation, just for asserts to make sure we restore animations correctly.
        // Setting it to true basically signals "I didn't forget that I have to manually restore animations here"
        bool animationRestoredAfterSave = false;

    private:
        Render::SpriteGroup* mCurrentAnim = nullptr;

        FAWorld::Tick mPlayingAnimDuration = 0;
        AnimationType mPlayingAnimType = AnimationType::Once;
        FAWorld::Tick mTicksSinceAnimStarted = 0;
        std::vector<int32_t> mFrameSequence;
    };
}
