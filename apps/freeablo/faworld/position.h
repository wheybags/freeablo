#pragma once
#include "misc/direction.h"
#include <cmath>
#include <misc/misc.h>
#include <utility>

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class Position
    {
    public:
        Position() = default;
        Position(int32_t x, int32_t y) : mCurrent(x, y) {}
        Position(int32_t x, int32_t y, Misc::Direction direction) : mCurrent(x, y), mDirection(direction) {}

        Position(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void update();                               ///< advances towards mNext
        std::pair<int32_t, int32_t> current() const; ///< where we are coming from
        bool isNear(const Position& other) const;
        std::pair<int32_t, int32_t> next() const; ///< where we are going to

        Misc::Direction getDirection() const { return mDirection; }
        void setDirection(Misc::Direction mDirection);

        bool isMoving() const { return mMoving; }
        int32_t getDist() const { return mDist; }

        void stop();
        void start();

    private:
        std::pair<int32_t, int32_t> mCurrent = std::make_pair(0, 0);
        int32_t mDist = 0; ///< percentage of the way there
        Misc::Direction mDirection = Misc::Direction::south;
        bool mMoving = false;
    };
}
