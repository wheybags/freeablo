#pragma once

#include "misc/direction.h"
#include "misc/misc.h"
#include "misc/point.h"

#include <cmath>
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
        explicit Position(Misc::Point point) : mCurrent(point) {}
        Position(Misc::Point point, Misc::Direction direction) : mCurrent(point), mDirection(direction) {}

        Position(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void update();               ///< advances towards mNext
        Misc::Point current() const; ///< where we are coming from
        bool isNear(const Position& other) const;
        Misc::Point next() const; ///< where we are going to

        Misc::Direction getDirection() const { return mDirection; }
        void setDirection(Misc::Direction mDirection);

        bool isMoving() const { return mMoving; }
        int32_t getDist() const { return mDist; }

        void stop();
        void start();

    private:
        Misc::Point mCurrent;
        int32_t mDist = 0; ///< percentage of the way there
        Misc::Direction mDirection = Misc::Direction::south;
        bool mMoving = false;
    };
}
