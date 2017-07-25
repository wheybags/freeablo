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

            void getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame);
            void playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick duration, AnimationType type);

        private:
            FARender::FASpriteGroup* mCurrentAnim = nullptr;
            
            FAWorld::Tick mPlayingAnimDuration = 0;
            AnimationType mPlayingAnimType;
            FAWorld::Tick mPlayingAnimStarted;
    };
}