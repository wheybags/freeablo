#include "animationplayer.h"

namespace FARender
{
    std::pair<FARender::FASpriteGroup*, int32_t> AnimationPlayer::getCurrentFrame()
    {
        if (mCurrentAnim == nullptr)
            return std::make_pair <FARender::FASpriteGroup*, int32_t >(nullptr, 0);


        FAWorld::Tick currentTick = FAWorld::World::get()->getCurrentTick();

        int32_t ticksIntoAnim = currentTick - mPlayingAnimStarted;
        float progress = ((float)ticksIntoAnim) / ((float)mPlayingAnimDuration);
        int32_t currentFrame = progress * mCurrentAnim->getAnimLength();

        if (currentFrame >= (int32_t)mCurrentAnim->getAnimLength())
        {
            if (mPlayingAnimType == AnimationType::Once)
            {
                playAnimation(nullptr, 0, AnimationType::Looped);
                return getCurrentFrame();
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

        return std::make_pair(mCurrentAnim, currentFrame);
    }

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick duration, AnimationPlayer::AnimationType type)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = duration;

        mPlayingAnimType = type;
        mPlayingAnimStarted = FAWorld::World::get()->getCurrentTick();
    }

    void AnimationPlayer::replaceAnimation(FARender::FASpriteGroup* anim)
    {
        mCurrentAnim = anim;
    }
}
