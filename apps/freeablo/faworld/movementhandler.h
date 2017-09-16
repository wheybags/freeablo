#pragma once

#include "position.h"
#include "world.h"
#include "gamelevel.h"

namespace FAWorld
{
    class GameLevel;

    class MovementHandler
    {
    public:
        MovementHandler(Tick pathRateLimit);

        std::pair<int32_t, int32_t> getDestination() const;
        void setDestination(std::pair<int32_t, int32_t> dest, bool adjacent = false);

        bool moving();
        Position getCurrentPosition() const;
        GameLevel* getLevel();
        void update(int32_t actorId);
        void teleport(GameLevel* level, Position pos);

    private:
        GameLevel* mLevel = nullptr;
        Position mCurrentPos;
        std::pair<int32_t, int32_t> mDestination;

        int32_t mCurrentPathIndex = 0;
        std::vector<std::pair<int32_t, int32_t>> mCurrentPath;
        Tick mLastRepathed = std::numeric_limits<Tick>::min();
        Tick mPathRateLimit;
        bool mAdjacent = false;


        template<class Stream>
        Serial::Error::Error faSerial(Stream& stream)
        {
            serialise_object(stream, mCurrentPos);
            serialise_int32(stream, mDestination.first);
            serialise_int32(stream, mDestination.second);

            int32_t levelIndex = -1;
            if (mLevel)
                levelIndex = mLevel->getLevelIndex();

            serialise_int32(stream, levelIndex);

            if (!stream.isWriting())
            {
                if (levelIndex == -1)
                    mLevel = nullptr;
                else
                    mLevel = World::get()->getLevel(levelIndex);
            }

            return Serial::Error::Success;
        }

        friend class Serial::WriteBitStream;
        friend class Serial::ReadBitStream;
    };
}
