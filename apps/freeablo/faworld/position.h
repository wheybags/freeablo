#ifndef POSITION_H
#define POSITION_H

#include <cmath>
#include <misc/misc.h>
#include <stdint.h>
#include <stdlib.h>
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
        Position(int32_t x, int32_t y, int32_t direction) : mCurrent(x, y), mDirection(direction) {}

        Position(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void update();                               ///< advances towards mNext
        std::pair<int32_t, int32_t> current() const; ///< where we are coming from
        bool isNear(const Position& other);
        std::pair<int32_t, int32_t> next() const; ///< where we are going to

        int32_t getDirection() const;
        void setDirection(int32_t mDirection);

        double distanceFrom(Position B);

        bool isMoving() const { return mMoving; }
        int32_t getDist() const { return mDist; }

        void stop()
        {
            mDist = 0;
            mMoving = false;
        }

        void start()
        {
            mDist = 0;
            mMoving = true;
        }

    private:
        std::pair<int32_t, int32_t> mCurrent = std::make_pair(0, 0);
        int32_t mDist = 0; ///< percentage of the way there
        int32_t mDirection = 0;
        bool mMoving = false;
    };
}

#endif
