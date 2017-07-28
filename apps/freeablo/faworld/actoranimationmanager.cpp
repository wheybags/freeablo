#include "actoranimationmanager.h"

#include "world.h"


namespace FAWorld
{
    ActorAnimationManager::ActorAnimationManager()
    {
        for (AnimState s = (AnimState)0; s < AnimState::ENUM_END; s = (AnimState)(((int32_t)s) + 1))
        {
            mAnimations[s] = FARender::getDefaultSprite();
            mAnimTimeMap[s] = World::getTicksInPeriod(0.5f);
        }

        mAnimTimeMap[AnimState::attack] = FAWorld::World::getTicksInPeriod(1.0f);
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
            playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);
    }
}