#include "actoranimationmanager.h"
#include "../fasavegame/gameloader.h"
#include "world.h"
#include <misc/assert.h>

namespace FAWorld
{
    ActorAnimationManager::ActorAnimationManager() { this->initAnimMaps(); }

    ActorAnimationManager::ActorAnimationManager(FASaveGame::GameLoader& loader)
    {
        this->initAnimMaps();

        mAnimationPlayer.load(loader);
        mPlayingAnim = AnimState(loader.load<uint8_t>());

        uint32_t numTimeMapEntries = loader.load<uint32_t>();
        for (uint32_t i = 0; i < numTimeMapEntries; i++)
        {
            AnimState type = AnimState(loader.load<uint8_t>());
            Tick time = loader.load<Tick>();

            mAnimTimeMap[size_t(type)] = time;
        }

        uint32_t idleFrameCount = loader.load<uint32_t>();
        mIdleFrameSequence.reserve(idleFrameCount);
        for (uint32_t i = 0; i < idleFrameCount; i++)
            mIdleFrameSequence.push_back(loader.load<int32_t>());

        mInterruptedAnimationState = AnimState(loader.load<uint8_t>());
        mInterruptedAnimationType = FARender::AnimationPlayer::AnimationType(loader.load<uint8_t>());
        mInterruptedAnimationFrame = loader.load<int32_t>();
    }

    void ActorAnimationManager::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("ActorAnimationManager", saver);

        mAnimationPlayer.save(saver);
        saver.save(uint8_t(mPlayingAnim));

        saver.save(uint32_t(AnimState::ENUM_END));
        for (AnimState s = (AnimState)0; s < AnimState::ENUM_END; s = AnimState(size_t(s) + 1))
        {
            saver.save(uint8_t(s));
            saver.save(mAnimTimeMap[size_t(s)]);
        }

        uint32_t idleFrameCount = mIdleFrameSequence.size();
        saver.save(idleFrameCount);

        for (uint32_t i = 0; i < idleFrameCount; i++)
            saver.save(mIdleFrameSequence[i]);

        saver.save(uint8_t(mInterruptedAnimationState));
        saver.save(uint8_t(mInterruptedAnimationType));
        saver.save(mInterruptedAnimationFrame);
    }

    void ActorAnimationManager::initAnimMaps()
    {
        for (AnimState s = (AnimState)0; s < AnimState::ENUM_END; s = AnimState(size_t(s) + 1))
        {
            mAnimations[size_t(s)] = nullptr;

            // Diablo 1 animations are 20 fps
            mAnimTimeMap[size_t(s)] = World::getTicksInPeriod(FixedPoint(1) / FixedPoint(20));
        }
    }

    std::pair<Render::SpriteGroup*, int32_t> ActorAnimationManager::getCurrentRealFrame() { return mAnimationPlayer.getCurrentFrame(); }

    void ActorAnimationManager::interruptAnimation(AnimState animation, FARender::AnimationPlayer::AnimationType type)
    {
        if (mPlayingAnim != animation && mPlayingAnim != AnimState::attack)
        {
            mInterruptedAnimationState = mPlayingAnim;
            mInterruptedAnimationFrame = mAnimationPlayer.getCurrentFrame().second;
            mInterruptedAnimationType = mAnimationPlayer.getCurrentAnimationType();
        }

        playAnimation(animation, type);
    }

    void ActorAnimationManager::playAnimation(AnimState animation, FARender::AnimationPlayer::AnimationType type)
    {
        mPlayingAnim = animation;
        mAnimationPlayer.playAnimation(mAnimations[size_t(animation)], mAnimTimeMap[size_t(animation)], type);
    }

    void ActorAnimationManager::playAnimation(AnimState animation, std::vector<int32_t> frameSequence)
    {
        mPlayingAnim = animation;
        mAnimationPlayer.playAnimation(mAnimations[size_t(animation)], mAnimTimeMap[size_t(animation)], frameSequence);
    }

    void ActorAnimationManager::setAnimationSprites(AnimState animation, Render::SpriteGroup* sprite)
    {
        if (mPlayingAnim == animation)
            mAnimationPlayer.replaceAnimation(sprite);

        mAnimations[size_t(animation)] = sprite;
    }

    void ActorAnimationManager::update()
    {
        mAnimationPlayer.update();
        Render::SpriteGroup* sprite = mAnimationPlayer.getCurrentFrame().first;

        // loop idle animation if we're not doing anything else
        if (sprite == nullptr)
        {
            if (mInterruptedAnimationState != AnimState::none)
            {
                mPlayingAnim = mInterruptedAnimationState;
                mAnimationPlayer.playAnimation(
                    mAnimations[size_t(mPlayingAnim)], mAnimTimeMap[size_t(mPlayingAnim)], mInterruptedAnimationType, mInterruptedAnimationFrame);

                mInterruptedAnimationState = AnimState::none;
                mInterruptedAnimationFrame = 0;
            }
            else
            {
                if (mIdleFrameSequence.empty())
                    playAnimation(AnimState::idle, FARender::AnimationPlayer::AnimationType::Looped);
                else
                    playAnimation(AnimState::idle, mIdleFrameSequence);
            }
        }
    }

    void ActorAnimationManager::setIdleFrameSequence(const std::vector<int32_t>& sequence) { mIdleFrameSequence = sequence; }

    int32_t ActorAnimationManager::getCurrentAnimationLength() const { return mAnimationPlayer.getAnimLength(); }

    void ActorAnimationManager::markAnimationsRestoredAfterGameLoad() { mAnimationPlayer.animationRestoredAfterSave = true; }
}
