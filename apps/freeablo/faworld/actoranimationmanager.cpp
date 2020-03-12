#include "actoranimationmanager.h"
#include "../fasavegame/gameloader.h"
#include "world.h"
#include <misc/assert.h>

namespace FAWorld
{
    ActorAnimationManager::ActorAnimationManager() { this->initAnimMaps(); }

    ActorAnimationManager::ActorAnimationManager(FASaveGame::GameLoader& loader) : mAnimationPlayer(loader)
    {
        this->initAnimMaps();

        mPlayingAnim = AnimState(loader.load<uint8_t>());

        uint32_t numAnimations = loader.load<uint32_t>();
        for (uint32_t i = 0; i < numAnimations; i++)
        {
            bool haveThisAnim = loader.load<bool>();

            if (haveThisAnim)
            {
                AnimState type = AnimState(loader.load<uint8_t>());
                std::string path = loader.load<std::string>();

                mAnimations[size_t(type)] = FARender::Renderer::get()->loadImage(path);
            }
        }

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

        saver.save(uint32_t(AnimState::ENUM_END)); // save the number of entries we're about to save
        for (AnimState s = (AnimState)0; s < AnimState::ENUM_END; s = AnimState(size_t(s) + 1))
        {
            bool haveThisAnim = mAnimations[size_t(s)]->isValid();
            saver.save(haveThisAnim);

            if (haveThisAnim)
            {
                std::string animPath = FARender::Renderer::get()->getPathForIndex(mAnimations[size_t(s)]->getCacheIndex());
                release_assert(animPath.size());

                saver.save(uint8_t(s));
                saver.save(animPath);
            }
        }

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
            mAnimations[size_t(s)] = FARender::getDefaultSprite();

            // Diablo 1 animations are 20 fps
            mAnimTimeMap[size_t(s)] = World::getTicksInPeriod(FixedPoint(1) / FixedPoint(20));
        }
    }

    std::pair<FARender::FASpriteGroup*, int32_t> ActorAnimationManager::getCurrentRealFrame() { return mAnimationPlayer.getCurrentFrame(); }

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

    void ActorAnimationManager::setAnimationSprites(AnimState animation, FARender::FASpriteGroup* sprite)
    {
        auto playingSprite = mAnimationPlayer.getCurrentFrame().first;

        if (playingSprite == mAnimations[size_t(animation)])
            mAnimationPlayer.replaceAnimation(sprite);

        mAnimations[size_t(animation)] = sprite;
    }

    void ActorAnimationManager::update()
    {
        mAnimationPlayer.update();
        auto sprite = mAnimationPlayer.getCurrentFrame().first;

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
}
