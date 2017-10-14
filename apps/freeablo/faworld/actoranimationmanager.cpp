#include "actoranimationmanager.h"

#include "world.h"


namespace FAWorld
{
    ActorAnimationManager::ActorAnimationManager()
    {
        for (AnimState s = (AnimState)0; s < AnimState::ENUM_END; s = (AnimState)(((int32_t)s) + 1))
        {
            mAnimations[s] = FARender::getDefaultSprite();
            mAnimTimeMap[s] = World::getTicksInPeriod(0.06f);
        }
    }

    AnimState ActorAnimationManager::getCurrentAnimation()
    {
        return mPlayingAnim;
    }

    std::pair<FARender::FASpriteGroup*, int32_t> ActorAnimationManager::getCurrentRealFrame()
    {
        return mAnimationPlayer.getCurrentFrame();
    }

    void ActorAnimationManager::playAnimation(AnimState animation, FARender::AnimationPlayer::AnimationType type)
    {
        mPlayingAnim = animation;
        mAnimationPlayer.playAnimation(mAnimations[animation], mAnimTimeMap[animation], type);
    }

    void ActorAnimationManager::playAnimation(AnimState animation, std::vector<int> frameSequence)
    {
        mPlayingAnim = animation;
        mAnimationPlayer.playAnimation(mAnimations[animation], mAnimTimeMap[animation], frameSequence);
    }

    void ActorAnimationManager::setAnimation(AnimState animation, FARender::FASpriteGroup* sprite)
    {
        auto playingSprite = mAnimationPlayer.getCurrentFrame().first;

        if (playingSprite == mAnimations[animation])
            mAnimationPlayer.replaceAnimation(sprite);

        mAnimations[animation] = sprite;
    }

    void ActorAnimationManager::update()
    {
        mAnimationPlayer.update();
        auto sprite = mAnimationPlayer.getCurrentFrame().first;

        // loop idle animation if we're not doing anything else
        if (sprite == nullptr)
        {
            if (mIdleFrameSequence.empty ())
                playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);
            else
                playAnimation(AnimState::idle, mIdleFrameSequence);
        }
    }

    void ActorAnimationManager::setIdleFrameSequence(const std::vector<int>& sequence)
    {
        mIdleFrameSequence = sequence;
    }
}
