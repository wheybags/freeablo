#pragma once

#include "position.h"
#include "world.h"

namespace FAWorld
{
    class GameLevel;

    class MovementHandler
    {
    public:
        MovementHandler(Tick pathRateLimit);

        std::pair<int32_t, int32_t> getDestination() const;
        void setDestination(std::pair<int32_t, int32_t> dest);

        bool moving();
        Position getCurrentPosition() const;
        GameLevel* getLevel();
        void update();
        void teleport(GameLevel* level, Position pos);

    private:
        GameLevel* mLevel = nullptr;
        Position mCurrentPos;
        std::pair<int32_t, int32_t> mDestination;

        int32_t mCurrentPathIndex = 0;
        std::vector<std::pair<int32_t, int32_t>> mCurrentPath;
        Tick mLastRepathed = std::numeric_limits<Tick>::min();
        Tick mPathRateLimit;


        template<class Stream>
        Serial::Error::Error faSerial(Stream& stream)
        {
            serialise_object(stream, mCurrentPos);
            serialise_int32(stream, mDestination.first);
            serialise_int32(stream, mDestination.second);

            return Serial::Error::Success;
        }

        friend class Serial::WriteBitStream;
        friend class Serial::ReadBitStream;
    };
}