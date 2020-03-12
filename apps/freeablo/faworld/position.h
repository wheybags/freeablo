#pragma once
#include "misc/direction.h"
#include "misc/misc.h"
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
        explicit Position(Misc::Point point = Misc::Point::zero(), Misc::Direction direction = Misc::Direction(Misc::Direction8::south));

        Position(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        FixedPoint update(FixedPoint moveDistance);
        Misc::Point current() const; ///< where we are coming from
        bool isNear(const Position& other) const;
        Misc::Point next() const; ///< where we are going to

        Misc::Direction getDirection() const { return mDirection; }
        void setDirection(Misc::Direction mDirection);

        bool isMoving() const { return mMovementType != MovementType::Stopped; }
        Vec2Fix getFractionalPos() const { return mFractionalPos; }

        void stopMoving();
        void gridMoveInDirection(Misc::Direction8 direction);
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
        Vec2Fix mFractionalPos;
        Misc::Direction mDirection = Misc::Direction(Misc::Direction8::south);
        MovementType mMovementType = MovementType::Stopped;
    };
}
