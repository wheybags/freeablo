#include "animationplayer.h"

namespace FARender
{
    void AnimationPlayer::getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame)
    {
        if (mCurrentAnim == nullptr)
        {
            sprite = nullptr;
            frame = 0;
            return;
        }

        FAWorld::Tick currentTick = FAWorld::World::get()->getCurrentTick();

        int32_t ticksIntoAnim = currentTick - mPlayingAnimStarted;
        float progress = ((float)ticksIntoAnim) / ((float)mPlayingAnimDuration);
        int32_t currentFrame = progress * mCurrentAnim->getAnimLength();

        if (currentFrame >= (int32_t)mCurrentAnim->getAnimLength())
        {
            if (mPlayingAnimType == AnimationType::Once)
            {
                playAnimation(nullptr, 0, AnimationType::Looped);
                getCurrentFrame(sprite, currentFrame);
                return;
            }
            else if (mPlayingAnimType == AnimationType::FreezeAtEnd)
            {
                currentFrame = mCurrentAnim->getAnimLength() - 1;
            }
            else if (mPlayingAnimType == AnimationType::Looped)
            {
                currentFrame = currentFrame % mCurrentAnim->getAnimLength();
            }
        }

        frame = currentFrame;
        sprite = mCurrentAnim;
    }

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick duration, AnimationPlayer::AnimationType type)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = duration;

        mPlayingAnimType = type;
        mPlayingAnimStarted = FAWorld::World::get()->getCurrentTick();
    }
}
