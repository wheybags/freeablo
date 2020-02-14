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
        explicit Position(Misc::Point point, int32_t speed = 250) : mCurrent(point), mSpeed(speed) {}
        Position(Misc::Point point, Misc::Direction direction, int32_t speed = 250) : mCurrent(point), mDirection(direction), mSpeed(speed) {}

        Position(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void update();               ///< advances towards mNext
        Misc::Point current() const; ///< where we are coming from
        bool isNear(const Position& other) const;
        Misc::Point next() const; ///< where we are going to

        Misc::Direction getDirection() const { return mDirection; }
        void setDirection(Misc::Direction mDirection);

        bool isMoving() const { return mMovementType != MovementType::Stopped; }
        Misc::Point getFractionalPos() const { return mFractionalPos; }

        int32_t getSpeed() const { return mSpeed; }
        void setSpeed(int32_t speed) { mSpeed = speed; }

        void stopMoving();
        void moveToPoint(const Misc::Point& dest);
        void setFreeMovement();

    private:
        enum class MovementType
        {
            Stopped,
            // GridLocked (Chebyshev) movement with a destination tile.
            // Movement to any neighboring tile takes the same time.
            GridLocked,
            // Free (Euclidean) movement in any direction.
            FreeMovement
        };

        Misc::Point mCurrent;
        Misc::Point mFractionalPos; ///< Higher resolution fraction percent between points (100ths)
        Misc::Direction mDirection = Misc::Direction(Misc::Direction8::south);
        Misc::Point mDest = {0, 0};
        int32_t mSpeed = 250;
        MovementType mMovementType = MovementType::Stopped;
    };
}
