#include "movementhandler.h"

#include "findpath.h"

namespace FAWorld
{
    MovementHandler::MovementHandler(Tick pathRateLimit) : mPathRateLimit(pathRateLimit)
    {}

    std::pair<int32_t, int32_t> MovementHandler::getDestination() const
    {
        return mDestination;
    }

    void MovementHandler::setDestination(std::pair<int32_t, int32_t> dest, bool adjacent)
    {
        mDestination = dest;
        mAdjacent = adjacent;
    }

    bool MovementHandler::moving()
    {
        return mCurrentPos.isMoving();
    }

    Position MovementHandler::getCurrentPosition() const
    {
        return mCurrentPos;
    }

    GameLevel* MovementHandler::getLevel()
    {
        return mLevel;
    }

    void MovementHandler::update(int32_t actorId)
    {
        if (mCurrentPos.getDist() == 0)
        {
            // if we have arrived, stop moving
            if (mCurrentPos.current() == mDestination)
            {
                mCurrentPos.stop();
                mCurrentPath.clear();
                mCurrentPathIndex = 0;
            }
            else
            {
                bool canRepath = std::abs(World::get()->getCurrentTick() - mLastRepathed) > mPathRateLimit;

                int32_t modNum = 3;
                canRepath = canRepath && ((World::get()->getCurrentTick() % modNum) == (actorId % modNum));

                bool needsRepath = true;
                mCurrentPos.stop();

                if (mCurrentPathIndex < (int32_t)mCurrentPath.size())
                {
                    // If our destination hasn't changed, or we can't repath, keep moving along our current path
                    if (mCurrentPath[mCurrentPath.size() - 1] == mDestination || !canRepath)
                    {
                        auto next = mCurrentPath[mCurrentPathIndex];

                        // Make sure nothing has moved into the way
                        if (mLevel->isPassable(next.first, next.second))
                        {
                            int32_t direction = Misc::getVecDir(Misc::getVec(mCurrentPos.current(), next));
                            mCurrentPos.setDirection(direction);
                            mCurrentPos.start();
                            needsRepath = false;
                            mCurrentPathIndex++;
                        }
                    }
                }

                if (needsRepath && canRepath)
                {
                    mLastRepathed = World::get()->getCurrentTick();

                    bool _;
                    mCurrentPath = std::move(pathFind(mLevel, mCurrentPos.current(), mDestination, _, mAdjacent));
                    mCurrentPathIndex = 0;

                    update(actorId);
                    return;
                }
            }
        }

        mCurrentPos.update();
    }

    void MovementHandler::teleport(GameLevel* level, Position pos)
    {
        mLevel = level;
        mCurrentPos = pos;
        mDestination = mCurrentPos.current();
    }
}
