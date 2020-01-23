#include "position.h"

#include "../fasavegame/gameloader.h"
#include "world.h"
#include <algorithm>
#include <cstdlib>

namespace FAWorld
{
    Position::Position(FASaveGame::GameLoader& loader)
    {
        mDirection = Misc::Direction(loader);
        mMovementType = static_cast<MovementType>(loader.load<int32_t>());
        mCurrent = Misc::Point(loader);
        mFractionalPos = Misc::Point(loader);
        mSpeed = loader.load<int32_t>();
        mDest = Misc::Point(loader);
    }

    void Position::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Position", saver);

        mDirection.save(saver);
        saver.save(static_cast<int32_t>(mMovementType));
        mCurrent.save(saver);
        mFractionalPos.save(saver);
        saver.save(mSpeed);
        mDest.save(saver);
    }

    void Position::update()
    {
        if (isMoving() && !mDirection.isNone())
        {
            auto vectorDist = FAWorld::World::getSecondsPerTick() * mSpeed;
            Misc::Point fractionalMovement;

            if (mMovementType == MovementType::GridLocked)
            {
                // GridLocked (Chebyshev) movement: movement to any neighboring tile takes the same time.
                auto distToPoint = (mDest - mCurrent) * 100 - mFractionalPos;

                // Move x and/or y by +- vectorDist.
                if (distToPoint.x != 0)
                    fractionalMovement.x = vectorDist.round() * ((distToPoint.x > 0) ? 1 : -1);
                if (distToPoint.y != 0)
                    fractionalMovement.y = vectorDist.round() * ((distToPoint.y > 0) ? 1 : -1);

                // Don't move past the destination point.
                if (std::abs(fractionalMovement.x) > std::abs(distToPoint.x))
                    fractionalMovement.x = distToPoint.x;
                if (std::abs(fractionalMovement.y) > std::abs(distToPoint.y))
                    fractionalMovement.y = distToPoint.y;
            }
            else
            {
                // Free (Euclidean) movement, requires trigonometry calculation.
                auto isometricDegrees = mDirection.getIsometricDegrees();
                fractionalMovement.x = (FixedPoint::cos_degrees(isometricDegrees) * vectorDist).round();
                fractionalMovement.y = (FixedPoint::sin_degrees(isometricDegrees) * vectorDist).round();
            }

            mFractionalPos += fractionalMovement;

            mCurrent += mFractionalPos / 100;
            mFractionalPos.x %= 100;
            mFractionalPos.y %= 100;

            // Possible improvement: When you're over 50% of the way to next position
            // you're technically in the next position.

            // Stop at destination.
            if (mMovementType == MovementType::GridLocked && mCurrent == mDest && mFractionalPos == Misc::Point(0, 0))
                stopMoving();
        }
    }

    void Position::setDirection(Misc::Direction mDirection) { this->mDirection = mDirection; }

    Misc::Point Position::current() const { return mCurrent; }

    bool Position::isNear(const Position& other) const
    {
        return std::max(std::abs(mCurrent.x - other.mCurrent.x), std::abs(mCurrent.y - other.mCurrent.y)) <= 1;
    }

    Misc::Point Position::next() const
    {
        if (!isMoving())
            return mCurrent;

        return Misc::getNextPosByDir(mCurrent, mDirection);
    }

    void Position::stopMoving() { mMovementType = MovementType::Stopped; }

    void Position::moveToPoint(const Misc::Point& dest)
    {
        mDest = dest;
        mMovementType = MovementType::GridLocked;
    }

    void Position::setFreeMovement() { mMovementType = MovementType::FreeMovement; }
}
