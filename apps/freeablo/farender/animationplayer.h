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
            AnimationPlayer(FARender::FASpriteGroup* idleAnim, FAWorld::Tick idleAnimDuration);

            void getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame);
            void playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick duration, AnimationType type);
            void setIdleAnimation(FARender::FASpriteGroup* idleAnim, FAWorld::Tick idleAnimDuration);

        private:
            FARender::FASpriteGroup* mIdleAnim = nullptr;
            FAWorld::Tick mIdleAnimDuration = 0;

            FARender::FASpriteGroup* mCurrentAnim = nullptr;
            
            FAWorld::Tick mPlayingAnimDuration = 0;
            AnimationType mPlayingAnimType;
            FAWorld::Tick mPlayingAnimStarted;
    };
}