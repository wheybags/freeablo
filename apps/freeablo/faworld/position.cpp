#include "position.h"

#include "../fasavegame/gameloader.h"
#include "world.h"
#include <algorithm>
#include <cstdlib>

namespace FAWorld
{
    Position::Position(FASaveGame::GameLoader& loader)
    {
        mDist = loader.load<int32_t>();
        mDirection = static_cast<Misc::Direction>(loader.load<int32_t>());
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
        saver.save(static_cast<int32_t>(mDirection));
        saver.save(mMoving);
        saver.save(mCurrent.first);
        saver.save(mCurrent.second);
    }

    void Position::update()
    {
        if (mMoving)
        {
            mDist += static_cast<int32_t>((FAWorld::World::getSecondsPerTick() * 250).intPart());

            if (mDist >= 100)
            {
                mCurrent = next();
                mDist = 0;
            }
        }
    }

    void Position::setDirection(Misc::Direction mDirection) { this->mDirection = mDirection; }

    std::pair<int32_t, int32_t> Position::current() const { return mCurrent; }

    bool Position::isNear(const Position& other) const
    {
        return std::max(std::abs(mCurrent.first - other.mCurrent.first), std::abs(mCurrent.second - other.mCurrent.second)) <= 1;
    }

    std::pair<int32_t, int32_t> Position::next() const
    {
        if (!mMoving)
            return mCurrent;

        return Misc::getNextPosByDir(mCurrent, mDirection);
    }

    void Position::stop()
    {
        mDist = 0;
        mMoving = false;
    }

    void Position::start()
    {
        mDist = 0;
        mMoving = true;
    }
}
