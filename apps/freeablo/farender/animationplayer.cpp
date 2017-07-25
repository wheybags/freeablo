#include "animationplayer.h"

namespace FARender
{
    AnimationPlayer::AnimationPlayer(FARender::FASpriteGroup* idleAnim, FAWorld::Tick idleAnimDuration)
    {
        mIdleAnim = idleAnim;
        mIdleAnimDuration = idleAnimDuration;

        playAnimation(mIdleAnim, mIdleAnimDuration, AnimationType::Looped);
    }

    void AnimationPlayer::getCurrentFrame(FARender::FASpriteGroup*& sprite, int32_t& frame)
    {
        FAWorld::Tick currentTick = FAWorld::World::get()->getCurrentTick();

        int32_t ticksIntoAnim = currentTick - mPlayingAnimStarted;
        float progress = ((float)ticksIntoAnim) / ((float)mPlayingAnimDuration);
        int32_t currentFrame = progress * mCurrentAnim->getAnimLength();

        if (currentFrame >= mCurrentAnim->getAnimLength())
        {
            if (mPlayingAnimType == AnimationType::Once)
            {
                playAnimation(mIdleAnim, mIdleAnimDuration, AnimationType::Looped);
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

    void AnimationPlayer::setIdleAnimation(FARender::FASpriteGroup* idleAnim, FAWorld::Tick idleAnimDuration)
    {
        bool restartIdle = mCurrentAnim == mIdleAnim && idleAnim != mIdleAnim;

        mIdleAnim = idleAnim;
        mIdleAnimDuration = idleAnimDuration;

        if (restartIdle)
            playAnimation(mIdleAnim, mIdleAnimDuration, AnimationType::Looped);
    }
}