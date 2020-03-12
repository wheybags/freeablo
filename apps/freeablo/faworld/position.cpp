#include "position.h"
#include "../fasavegame/gameloader.h"
#include "world.h"
#include <algorithm>
#include <cstdlib>

namespace FAWorld
{
    Position::Position(Misc::Point point, Misc::Direction direction) : mCurrent(point), mFractionalPos(Vec2Fix(point)), mDirection(direction) {}

    Position::Position(FASaveGame::GameLoader& loader)
    {
        mDirection = Misc::Direction(loader);
        mMovementType = static_cast<MovementType>(loader.load<int32_t>());
        mCurrent = Misc::Point(loader);
        mFractionalPos = Vec2Fix(loader);
    }

    void Position::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("Position", saver);

        mDirection.save(saver);
        saver.save(static_cast<int32_t>(mMovementType));
        mCurrent.save(saver);
        mFractionalPos.save(saver);
    }

    FixedPoint Position::update(FixedPoint moveDistance)
    {
        if (isMoving())
        {
            Vec2Fix vectorToDest;
            if (mMovementType == MovementType::GridLocked)
            {
                vectorToDest = Vec2Fix(next()) - mFractionalPos;
            }
            else
            {
                // https://wheybags.gitlab.io/jarulfs-guide/#spell-and-arrow-speeds
                // Techhnically, this isn't quite correct, as in the original game projectiles essentially
                // moved through the world in "screen coordinates", and we move in the game's isometric
                // space, but honestly I think this way is better anyway.
                FixedPoint isometricDegrees = mDirection.getIsometricDegrees();
                vectorToDest = Vec2Fix(FixedPoint::cos_degrees(isometricDegrees), FixedPoint::sin_degrees(isometricDegrees));
            }

            Vec2Fix movement = vectorToDest;
            movement.normalise();
            movement *= moveDistance;

            mFractionalPos += movement;

            if (mMovementType == MovementType::GridLocked)
            {
                FixedPoint vectorToDestMagnitudeSquared = vectorToDest.magnitudeSquared();
                if (movement.magnitudeSquared() >= vectorToDestMagnitudeSquared)
                {
                    Misc::Point nextPositon = next();
                    mCurrent = nextPositon;
                    mFractionalPos = Vec2Fix(nextPositon);
                    stopMoving();

                    return moveDistance - vectorToDestMagnitudeSquared.sqrt();
                }
            }
            else
            {
                mCurrent = Misc::Point(mFractionalPos);
            }

            return 0;
        }

        return moveDistance;
    }

    void Position::setDirection(Misc::Direction mDirection) { this->mDirection = mDirection; }

    Misc::Point Position::current() const { return mCurrent; }

    bool Position::isNear(const Position& other) const
    {
        return std::max(std::abs(current().x - other.current().x), std::abs(current().y - other.current().y)) <= 1;
    }

    Misc::Point Position::next() const
    {
        if (!isMoving())
            return current();

        return Misc::getNextPosByDir(current(), mDirection);
    }

    void Position::stopMoving() { mMovementType = MovementType::Stopped; }

    void Position::gridMoveInDirection(Misc::Direction8 direction)
    {
        setDirection(direction);
        mMovementType = MovementType::GridLocked;
    }

    void Position::setFreeMovement() { mMovementType = MovementType::FreeMovement; }
}
