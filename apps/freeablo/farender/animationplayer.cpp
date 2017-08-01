#include "animationplayer.h"

namespace FARender
{
    std::pair<FARender::FASpriteGroup*, int32_t> AnimationPlayer::getCurrentFrame()
    {
        if (mCurrentAnim == nullptr)
            return std::make_pair <FARender::FASpriteGroup*, int32_t >(nullptr, 0);

        float progress = ((float)mTicksSinceAnimStarted) / ((float)mPlayingAnimDuration);
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
        mTicksSinceAnimStarted = 0;
    }

    void AnimationPlayer::replaceAnimation(FARender::FASpriteGroup* anim)
    {
        mCurrentAnim = anim;
    }

    void AnimationPlayer::update()
    {
        mTicksSinceAnimStarted++;
    }
}
