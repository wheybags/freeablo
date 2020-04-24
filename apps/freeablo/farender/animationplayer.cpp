#include "animationplayer.h"
#include "../fasavegame/gameloader.h"
#include <misc/assert.h>
#include <misc/fixedpoint.h>
#include <render/spritegroup.h>

namespace FARender
{
    void AnimationPlayer::load(FASaveGame::GameLoader& loader)
    {
        mPlayingAnimDuration = loader.load<FAWorld::Tick>();
        mPlayingAnimType = AnimationType(loader.load<uint8_t>());
        mTicksSinceAnimStarted = loader.load<FAWorld::Tick>();

        uint32_t frameSequenceSize = loader.load<uint32_t>();

        mFrameSequence.reserve(frameSequenceSize);
        for (uint32_t i = 0; i < frameSequenceSize; i++)
            mFrameSequence.push_back(loader.load<int32_t>());

        loader.addFunctionToRunAtEnd([this]() { release_assert(animationRestoredAfterSave); });
    }

    void AnimationPlayer::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("AnimationPlayer", saver);

        saver.save(mPlayingAnimDuration);
        saver.save(uint8_t(mPlayingAnimType));
        saver.save(mTicksSinceAnimStarted);

        {
            Serial::ScopedCategorySaver frameSequenceCategory("FrameSequence", saver);

            uint32_t frameSequenceSize = mFrameSequence.size();
            saver.save(frameSequenceSize);

            for (uint32_t i = 0; i < frameSequenceSize; i++)
                saver.save(mFrameSequence[i]);
        }
    }

    std::pair<Render::SpriteGroup*, int32_t> AnimationPlayer::getCurrentFrame() const
    {
        if (mCurrentAnim == nullptr)
            return std::make_pair<Render::SpriteGroup*, int32_t>(nullptr, 0);

        int32_t currentFrame;
        FixedPoint progress = FixedPoint(mTicksSinceAnimStarted) / FixedPoint(mPlayingAnimDuration);

        if (mPlayingAnimType != AnimationType::BySequence)
        {
            currentFrame = int32_t(progress.intPart());

            if (currentFrame >= int32_t(mCurrentAnim->getAnimationLength()))
            {
                switch (mPlayingAnimType)
                {
                    case AnimationType::Once:
                        return std::make_pair<Render::SpriteGroup*, int32_t>(nullptr, 0);
                    case AnimationType::FreezeAtEnd:
                        currentFrame = mCurrentAnim->getAnimationLength() - 1;
                        break;
                    case AnimationType::Looped:
                        currentFrame = currentFrame % mCurrentAnim->getAnimationLength();
                        break;
                    case AnimationType::BySequence:
                    // handled below
                    case AnimationType::ENUM_END:
                        break;
                }
            }
        }
        else
            currentFrame = mFrameSequence[size_t(progress.intPart()) % mFrameSequence.size()];

        return std::make_pair(mCurrentAnim, currentFrame);
    }

    void AnimationPlayer::playAnimation(Render::SpriteGroup* anim, FAWorld::Tick frameDuration, AnimationPlayer::AnimationType type, int32_t startFrame)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = frameDuration;

        mPlayingAnimType = type;
        mTicksSinceAnimStarted = frameDuration * startFrame;
    }

    void AnimationPlayer::playAnimation(Render::SpriteGroup* anim, FAWorld::Tick frameDuration, std::vector<int32_t> frameSequence)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = frameDuration;
        mPlayingAnimType = AnimationType::BySequence;
        mFrameSequence = std::move(frameSequence);
    }

    void AnimationPlayer::replaceAnimation(Render::SpriteGroup* anim)
    {
        if (anim)
            release_assert(mPlayingAnimDuration);

        mCurrentAnim = anim;
    }

    void AnimationPlayer::stopAnimation() { playAnimation(nullptr, 0, AnimationType::Looped); }

    void AnimationPlayer::update()
    {
        mTicksSinceAnimStarted++;

        if (mCurrentAnim == nullptr)
            return;

        if (mPlayingAnimType == AnimationType::Once)
        {
            FixedPoint progress = FixedPoint(mTicksSinceAnimStarted) / FixedPoint(mPlayingAnimDuration);
            int32_t currentFrame = int32_t(progress.intPart());

            if (currentFrame >= mCurrentAnim->getAnimationLength())
                stopAnimation();
        }
    }

    int32_t AnimationPlayer::getAnimLength() const
    {
        if (!mCurrentAnim)
            return -1;
        return mCurrentAnim->getAnimationLength();
    }

    struct nk_image AnimationPlayer::getCurrentNkImage()
    {
        auto p = getCurrentFrame();
        return p.first->getNkImage(p.second);
    }
}
