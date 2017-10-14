#pragma once

#include <unordered_map>

#include "../farender/animationplayer.h"

#include <misc/stdhashes.h>

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
        void playAnimation(AnimState animation, std::vector<int> frameSequence);
        void setAnimation(AnimState animation, FARender::FASpriteGroup* sprite);

        void update();
        void setIdleFrameSequence(const std::vector<int>& sequence);

    private:
        AnimState mPlayingAnim = AnimState::none;
        FARender::AnimationPlayer mAnimationPlayer;

        std::unordered_map<AnimState, FARender::FASpriteGroup*, EnumClassHash> mAnimations;
        std::map<AnimState, Tick> mAnimTimeMap;
        std::vector<int> mIdleFrameSequence;

        template <class Stream>
        Serial::Error::Error faSerial(Stream& stream)
        {
            serialise_enum(stream, AnimState, mPlayingAnim);
            serialise_object(stream, mAnimationPlayer);

            int32_t numAnims = mAnimations.size();
            serialise_int32(stream, numAnims);


            if (stream.isWriting())
            {
                for (auto it = mAnimations.begin(); it != mAnimations.end(); it++)
                {
                    AnimState state = it->first;
                    int32_t cacheIndex = it->second->getCacheIndex();

                    serialise_enum(stream, AnimState, state);
                    serialise_int32(stream, cacheIndex);

                    Tick timing = mAnimTimeMap[state];
                    serialise_int(stream, 0, FAWorld::MAX_TICK, timing);
                }
            }
            else
            {
                auto netManager = Engine::NetManager::get();

                mAnimations.clear();
                mAnimTimeMap.clear();

                for (int32_t i = 0; i < numAnims; i++)
                {
                    AnimState state = AnimState::none;
                    int32_t cacheIndex = 0;
                    Tick timing = 0;

                    serialise_enum(stream, AnimState, state);
                    serialise_int32(stream, cacheIndex);
                    serialise_int(stream, 0, FAWorld::MAX_TICK, timing);

                    FARender::FASpriteGroup* sprite = netManager->getServerSprite(cacheIndex);

                    mAnimations[state] = sprite;
                    mAnimTimeMap[state] = timing;
                }
            }

            // just a little test to make sure the dictionaries went through correctly
            int32_t testVal = 567;
            serialise_int32(stream, testVal);
            if (!stream.isWriting())
                assert(testVal == 567);


            return Serial::Error::Success;
        }

        friend class Serial::WriteBitStream;
        friend class Serial::ReadBitStream;
    };
}
