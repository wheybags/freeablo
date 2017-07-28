#pragma once

#include <unordered_map>

#include "../farender/animationplayer.h"

namespace FAWorld
{
    enum class AnimState
    {
        walk,
        idle,
        attack,
        dead,
        hit,
        none,
        ENUM_END // always leave this as the last entry, and don't set explicit values for any of the entries
    };

    class ActorAnimationManager
    {
    public:
        ActorAnimationManager();

        AnimState getCurrentAnimation();
        std::pair<FARender::FASpriteGroup*, int32_t> getCurrentRealFrame();

        void playAnimation(AnimState animation, FARender::AnimationPlayer::AnimationType type);
        void setAnimation(AnimState animation, FARender::FASpriteGroup* sprite);

        void update();

    private:
        AnimState mPlayingAnim = AnimState::none;
        FARender::AnimationPlayer mAnimationPlayer;

        std::unordered_map<AnimState, FARender::FASpriteGroup*> mAnimations;
        std::map<AnimState, Tick> mAnimTimeMap;
    };
}