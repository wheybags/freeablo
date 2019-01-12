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
        mCurrent = Misc::Point(loader);
    }

    void Position::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Position", saver);

        saver.save(mDist);
        saver.save(static_cast<int32_t>(mDirection));
        saver.save(mMoving);
        mCurrent.save(saver);
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
                mMoving = false;
            }
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
