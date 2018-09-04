#include "movementhandler.h"
#include "../fasavegame/gameloader.h"
#include "findpath.h"
#include <misc/vec2fix.h>

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
        mDestination = Misc::Point(loader);

        mCurrentPathIndex = loader.load<int32_t>();
        uint32_t pathSize = loader.load<uint32_t>();
        mCurrentPath.reserve(pathSize);
        for (uint32_t i = 0; i < pathSize; i++)
        {
            mCurrentPath.emplace_back(Misc::Point(loader));
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
        mDestination.save(saver);

        saver.save(mCurrentPathIndex);

        uint32_t pathSize = mCurrentPath.size();
        saver.save(pathSize);
        for (const auto& item : mCurrentPath)
        {
            item.save(saver);
        }

        saver.save(mLastRepathed);
        saver.save(mPathRateLimit);
        saver.save(mAdjacent);
    }

    Misc::Point MovementHandler::getDestination() const { return mDestination; }

    void MovementHandler::setDestination(Misc::Point dest, bool adjacent)
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
                        if (mLevel->isPassable(next))
                        {
                            auto vec = Vec2Fix(next.x, next.y) - Vec2Fix(mCurrentPos.current().x, mCurrentPos.current().y);
                            Misc::Direction direction = vec.getIsometricDirection();

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

                    if (!mCurrentPath.empty())
                        mDestination = mCurrentPath.back();

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
