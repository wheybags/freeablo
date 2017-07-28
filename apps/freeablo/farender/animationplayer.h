#pragma once

#include "renderer.h"
#include "../faworld/world.h"

namespace FARender
{
    class AnimationPlayer
    {
        public:

            enum class AnimationType
            {
                Looped,
                Once,
                FreezeAtEnd
            };

            AnimationPlayer() {}

            std::pair<FARender::FASpriteGroup*, int32_t> getCurrentFrame();
            void playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick duration, AnimationType type);

            //!
            //! Simply replaces the currently running animation.
            //! The difference from playAnimation is that it will not start at the start of the new animation,
            //! but at whatever point the animation running before it was at, ie it "replaces" instead of
            //! playing a new animation
            void replaceAnimation(FARender::FASpriteGroup* anim);

        private:
            FARender::FASpriteGroup* mCurrentAnim = nullptr;
            
            FAWorld::Tick mPlayingAnimDuration = 0;
            AnimationType mPlayingAnimType;
            FAWorld::Tick mPlayingAnimStarted;
    };
}