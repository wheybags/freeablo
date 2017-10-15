#include "animationplayer.h"

namespace FARender
{
    std::pair<FARender::FASpriteGroup*, int32_t> AnimationPlayer::getCurrentFrame()
    {
        if (mCurrentAnim == nullptr)
            return std::make_pair <FARender::FASpriteGroup*, int32_t >(nullptr, 0);

        int32_t currentFrame;
        float progress = ((float)mTicksSinceAnimStarted) / ((float)mPlayingAnimDuration);

        if (mPlayingAnimType != AnimationType::BySequence)
        {
            currentFrame = progress;

            if (currentFrame >= (int32_t)mCurrentAnim->getAnimLength())
            {
                switch (mPlayingAnimType)
                {
                case AnimationType::Once:
                    playAnimation(nullptr, 0, AnimationType::Looped);
                    return getCurrentFrame();
                case AnimationType::FreezeAtEnd:
                    currentFrame = mCurrentAnim->getAnimLength() - 1;
                    break;
                case AnimationType::Looped:
                    currentFrame = currentFrame % mCurrentAnim->getAnimLength();
                    break;
                case AnimationType::BySequence:
                    // handled below
                case AnimationType::ENUM_END:
                    break;
                }
            }
        }
        else
            currentFrame = mFrameSequence[static_cast<int32_t> (progress) % mFrameSequence.size ()];

        return std::make_pair(mCurrentAnim, currentFrame);
    }

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick frameDuration, AnimationPlayer::AnimationType type)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = frameDuration;

        mPlayingAnimType = type;
        mTicksSinceAnimStarted = 0;
    }

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick frameDuration, std::vector<int> frameSequence)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = frameDuration;
        mPlayingAnimType = AnimationType::BySequence;
        mFrameSequence = frameSequence;
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
