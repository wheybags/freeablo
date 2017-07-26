#include "movementhandler.h"

#include "findpath.h"

namespace FAWorld
{
    std::pair<int32_t, int32_t> MovementHandler::getDestination() const
    {
        return mDestination;
    }

    void MovementHandler::setDestination(std::pair<int32_t, int32_t> dest)
    {
        mDestination = dest;
    }

    bool MovementHandler::moving()
    {
        return mCurrentPos.mMoving;
    }

    Position MovementHandler::getCurrentPosition() const
    {
        return mCurrentPos;
    }

    void MovementHandler::update()
    {
        if (mCurrentPos.mDist == 0)
        {
            // if we have arrived, stop moving
            if (mCurrentPos.current() == mDestination)
            {
                mCurrentPos.mMoving = false;
                mCurrentPath.clear();
                mCurrentPathIndex = 0;
            }
            else
            {
                bool canRepath = mTickSinceLastPath > World::getTicksInPeriod(1.0f);
                bool needsRepath = true;
                mCurrentPos.mMoving = false;

                if (mCurrentPathIndex < mCurrentPath.size())
                {
                    //|| mCurrentPath[mCurrentPath.size() - 1] != mDestination)
                    //needsRepath = true;

                    // If our destination hasn't changed, or we can't repath, keep pathing towards it
                    if (mCurrentPath[mCurrentPath.size() - 1] == mDestination || !canRepath)
                    {
                        auto next = mCurrentPath[mCurrentPathIndex];

                        // Make sure nothing has moved into the way
                        if (mLevel->isPassable(next.first, next.second))
                        {
                            int32_t direction = Misc::getVecDir(Misc::getVec(mCurrentPos.current(), next));
                            mCurrentPos.setDirection(direction);
                            mCurrentPos.mMoving = true;
                            needsRepath = false;
                            mCurrentPathIndex++;
                        }
                    }
                }

                if (needsRepath && canRepath)
                {
                    mTickSinceLastPath = 0;

                    bool _;
                    mCurrentPath = std::move(pathFind(mLevel, mCurrentPos.current(), mDestination, _));
                    mCurrentPathIndex = 0;

                    update();
                    return;
                }
            }
        }

        mCurrentPos.update();
        mTickSinceLastPath++;
    }

    void MovementHandler::teleport(GameLevel* level, Position pos)
    {
        mLevel = level;
        mCurrentPos = pos;
        mDestination = mCurrentPos.current();
    }
}