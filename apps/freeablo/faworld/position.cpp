#include "position.h"

#include "../fasavegame/gameloader.h"
#include "world.h"
#include <algorithm>

namespace FAWorld
{
    Position::Position(FASaveGame::GameLoader& loader)
    {
        mDist = loader.load<int32_t>();
        mDirection = loader.load<int32_t>();
        mMoving = loader.load<bool>();

        int32_t first, second;
        first = loader.load<int32_t>();
        second = loader.load<int32_t>();
        mCurrent = std::make_pair(first, second);
    }

    void Position::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Position", saver);

        saver.save(mDist);
        saver.save(mDirection);
        saver.save(mMoving);
        saver.save(mCurrent.first);
        saver.save(mCurrent.second);
    }

    void Position::update()
    {
        if (mMoving)
        {
            mDist += static_cast<int32_t>(FAWorld::World::getSecondsPerTick() * 250);

            if (mDist >= 100)
            {
                mCurrent = next();
                mDist = 0;
            }
        }
    }

    int32_t Position::getDirection() const { return mDirection; }

    void Position::setDirection(int32_t mDirection)
    {
        if (mDirection >= 0)
            this->mDirection = mDirection;
    }

    std::pair<int32_t, int32_t> Position::current() const { return mCurrent; }

    bool Position::isNear(const Position& other)
    {
        return std::max(abs(mCurrent.first - other.mCurrent.first), abs(mCurrent.second - other.mCurrent.second)) <= 1;
    }

    double Position::distanceFrom(Position B)
    {
        int dx = mCurrent.first - B.mCurrent.first;
        int dy = mCurrent.second - B.mCurrent.second;

        double x = pow(dx, 2.0);
        double y = pow(dy, 2.0);
        double distance = sqrt(x + y);

        return distance;
    }

    std::pair<int32_t, int32_t> Position::next() const
    {
        if (!mMoving)
            return mCurrent;

        return Misc::getNextPosByDir(mCurrent, mDirection);
    }
}
