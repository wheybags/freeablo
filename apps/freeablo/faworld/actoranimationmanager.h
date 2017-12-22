
#pragma once

#include <unordered_map>

#include "../farender/animationplayer.h"

#include <misc/stdhashes.h>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    enum class AnimState : uint8_t
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
        ActorAnimationManager(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void initAnimMaps();

        AnimState getCurrentAnimation();
        AnimState getInterruptedAnimation() { return mInterruptedAnimationState; }

        std::pair<FARender::FASpriteGroup*, int32_t> getCurrentRealFrame();

        void playAnimation(AnimState animation, FARender::AnimationPlayer::AnimationType type);
        void playAnimation(AnimState animation, std::vector<int32_t> frameSequence);
        void interruptAnimation(AnimState animation, FARender::AnimationPlayer::AnimationType type);

        void setAnimation(AnimState animation, FARender::FASpriteGroup* sprite);

        void update();
        void setIdleFrameSequence(const std::vector<int32_t>& sequence);

    private:
        FARender::AnimationPlayer mAnimationPlayer;
        AnimState mPlayingAnim = AnimState::none;

        // TODO: some template class for an array of T with EnumType::ENUM_END size array, to eliminate the casting used
        // for accessing these two arrays (call it EnumMap or something)
        FARender::FASpriteGroup* mAnimations[size_t(AnimState::ENUM_END)]; ///< "map" from AnimState to animation
        Tick mAnimTimeMap[size_t(AnimState::ENUM_END)];                    ///< "map" from AnimState to Tick

        std::vector<int32_t> mIdleFrameSequence;

        // TODO: we could probably do with making this a stack, but it's good enough for now
        // At time of writing (5/Nov/17), it's just used for hit animations, which don't need a stack
        AnimState mInterruptedAnimationState = AnimState::none;
        FARender::AnimationPlayer::AnimationType mInterruptedAnimationType;
        int32_t mInterruptedAnimationFrame = 0;
    };
}

