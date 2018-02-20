#include "movementhandler.h"

#include "../fasavegame/gameloader.h"
#include "findpath.h"

namespace FAWorld
{
    MovementHandler::MovementHandler(Tick pathRateLimit) : mPathRateLimit(pathRateLimit) {}

    MovementHandler::MovementHandler(FASaveGame::GameLoader& loader)
    {
        int32_t levelIndex = loader.load<int32_t>();
        if (levelIndex != -1)
        {
            // get the level at the end, because it doesn't exist yet
            World* world = loader.currentlyLoadingWorld;
            loader.addFunctionToRunAtEnd([this, levelIndex, world]() { this->mLevel = world->getLevel(levelIndex); });
        }

        mCurrentPos = Position(loader);

        int32_t first, second;
        first = loader.load<int32_t>();
        second = loader.load<int32_t>();
        mDestination = std::make_pair(first, second);

        mCurrentPathIndex = loader.load<int32_t>();
        uint32_t pathSize = loader.load<uint32_t>();
        mCurrentPath.reserve(pathSize);
        for (uint32_t i = 0; i < pathSize; i++)
        {
            first = loader.load<int32_t>();
            second = loader.load<int32_t>();
            mCurrentPath.push_back(std::make_pair(first, second));
        }

        mLastRepathed = loader.load<Tick>();
        mPathRateLimit = loader.load<Tick>();
        mAdjacent = loader.load<bool>();
    }

    void MovementHandler::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("MovementHandler", saver);

        int32_t levelIndex = -1;
        if (mLevel)
            levelIndex = mLevel->getLevelIndex();

        saver.save(levelIndex);
        mCurrentPos.save(saver);
        saver.save(mDestination.first);
        saver.save(mDestination.second);

        saver.save(mCurrentPathIndex);

        uint32_t pathSize = mCurrentPath.size();
        saver.save(pathSize);
        for (const auto& item : mCurrentPath)
        {
            saver.save(item.first);
            saver.save(item.second);
        }

        saver.save(mLastRepathed);
        saver.save(mPathRateLimit);
        saver.save(mAdjacent);
    }

    std::pair<int32_t, int32_t> MovementHandler::getDestination() const { return mDestination; }

    void MovementHandler::setDestination(std::pair<int32_t, int32_t> dest, bool adjacent)
    {
        mDestination = dest;
        mAdjacent = adjacent;
    }

    bool MovementHandler::moving() { return mCurrentPos.isMoving(); }

    GameLevel* MovementHandler::getLevel() { return mLevel; }

    void MovementHandler::update(int32_t actorId)
    {
        debug_assert(mLevel);

        if (mCurrentPos.getDist() == 0)
        {
            if (!positionReachedSent)
            {
                positionReached(mCurrentPos.current());
                positionReachedSent = true;
            }
            // if we have arrived, stop moving
            if (mCurrentPos.current() == mDestination)
            {
                mCurrentPos.stop();
                mCurrentPath.clear();
                mCurrentPathIndex = 0;
            }
            else
            {
                bool canRepath = std::abs(mLevel->getWorld()->getCurrentTick() - mLastRepathed) > mPathRateLimit;

                int32_t modNum = 3;
                canRepath = canRepath && ((mLevel->getWorld()->getCurrentTick() % modNum) == (actorId % modNum));

                bool needsRepath = true;
                mCurrentPos.stop();

                if (mCurrentPathIndex < (int32_t)mCurrentPath.size())
                {
                    // If our destination hasn't changed, or we can't repath, keep moving along our current path
                    if (mCurrentPath.back() == mDestination || !canRepath)
                    {
                        auto next = mCurrentPath[mCurrentPathIndex];

                        // Make sure nothing has moved into the way
                        if (mLevel->isPassable(next.first, next.second))
                        {
                            auto direction = Misc::getVecDir(Misc::getVec(mCurrentPos.current(), next));
                            positionReachedSent = false;
                            mCurrentPos.setDirection(direction);
                            mCurrentPos.start();
                            needsRepath = false;
                            mCurrentPathIndex++;
                        }
                    }
                }

                if (needsRepath && canRepath)
                {
                    mLastRepathed = mLevel->getWorld()->getCurrentTick();

                    bool _;
                    mCurrentPath = pathFind(mLevel, mCurrentPos.current(), mDestination, _, mAdjacent);
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

    void MovementHandler::setDirection(Misc::Direction direction) { mCurrentPos.setDirection(direction); }
}
