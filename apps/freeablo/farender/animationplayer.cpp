#include "animationplayer.h"
#include "../fasavegame/gameloader.h"

namespace FARender
{
    AnimationPlayer::AnimationPlayer(FASaveGame::GameLoader& loader)
    {
        bool hasCurrentAnim = loader.load<bool>();

        if (hasCurrentAnim)
            mCurrentAnim = Renderer::get()->loadImage(loader.load<std::string>());

        mPlayingAnimDuration = loader.load<FAWorld::Tick>();
        mPlayingAnimType = AnimationType(loader.load<uint8_t>());
        mTicksSinceAnimStarted = loader.load<FAWorld::Tick>();

        uint32_t frameSequenceSize = loader.load<uint32_t>();

        mFrameSequence.reserve(frameSequenceSize);
        for (uint32_t i = 0; i < frameSequenceSize; i++)
            mFrameSequence.push_back(loader.load<int32_t>());
    }

    void AnimationPlayer::save(FASaveGame::GameSaver& saver)
    {
        bool hasCurrentAnim = mCurrentAnim != nullptr;
        saver.save(hasCurrentAnim);

        if (hasCurrentAnim)
        {
            std::string spritePath = Renderer::get()->getPathForIndex(mCurrentAnim->getCacheIndex());
            assert(spritePath.size());
            saver.save(spritePath);
        }

        saver.save(mPlayingAnimDuration);
        saver.save(uint8_t(mPlayingAnimType));
        saver.save(mTicksSinceAnimStarted);

        uint32_t frameSequenceSize = mFrameSequence.size();
        saver.save(frameSequenceSize);

        for (uint32_t i = 0; i < frameSequenceSize; i++)
            saver.save(mFrameSequence[i]);
    }

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

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick frameDuration, AnimationPlayer::AnimationType type, int32_t startFrame)
    {
        mCurrentAnim = anim;
        mPlayingAnimDuration = frameDuration;

        mPlayingAnimType = type;
        mTicksSinceAnimStarted = frameDuration * startFrame;
    }

    void AnimationPlayer::playAnimation(FARender::FASpriteGroup* anim, FAWorld::Tick frameDuration, std::vector<int32_t> frameSequence)
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
