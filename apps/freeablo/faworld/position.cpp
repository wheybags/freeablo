#include "position.h"

#include "world.h"

namespace FAWorld
{
    Position::Position() : mDist(0), mDirection(0), mPath(std::vector<std::pair<int32_t, int32_t>>()), mGoal(std::pair<int32_t, int32_t>(0,0)), mIndex(-1), mMoving(false),
        mCurrent(std::make_pair(0, 0)) {}

    Position::Position(int32_t x, int32_t y) : mDist(0), mDirection(0), mPath(std::vector<std::pair<int32_t, int32_t>>()), mGoal(std::pair<int32_t, int32_t>(0, 0)), mIndex(-1), mMoving(false),
        mCurrent(std::make_pair(x, y)) {}

    Position::Position(int32_t x, int32_t y, int32_t direction) : mDist(0), mDirection(direction), mPath(std::vector<std::pair<int32_t, int32_t>>()), mGoal(std::pair<int32_t, int32_t>(0, 0)), mIndex(-1), mMoving(false),
        mCurrent(std::make_pair(x, y)) {}

    void Position::update()
    {
        if (mMoving)
        {
            mDist += static_cast<int32_t> (FAWorld::World::getSecondsPerTick() * 250);

            if (mDist >= 100)
            {
                if (mPath.size() && mIndex != -1)
                {
                    mCurrent = pathNext(true);
                }
                else {
                    mCurrent = next();
                }
                mDist = 0;
            }
        }
    }

    std::pair<int32_t, int32_t> Position::current() const
    {
        return mCurrent;
    }

    double Position::distanceFrom(const Position &B)
    {
        int dx = mCurrent.first - B.mCurrent.first;
        int dy = mCurrent.second - B.mCurrent.second;

        double x = dx * dx;
        double y = dy * dy;
        double distance = sqrt(x + y);

        return distance;
    }

    std::pair<int32_t, int32_t> Position::next() const
    {
        if (!mMoving)
            return mCurrent;

        std::pair<int32_t, int32_t> retval = mCurrent;

        switch (mDirection)
        {
        case 0:
        {
            retval.first++;
            retval.second++;
            break;
        }

        case 7:
        {
            retval.first++;
            break;
        }

        case 6:
        {
            retval.first++;
            retval.second--;
            break;
        }

        case 5:
        {
            retval.second--;
            break;
        }

        case 4:
        {
            retval.first--;
            retval.second--;
            break;
        }

        case 3:
        {
            retval.first--;
            break;
        }

        case 2:
        {
            retval.first--;
            retval.second++;
            break;
        }

        case 1:
        {
            retval.second++;
            break;
        }

        default:
        {
            break;
        }
        }

        return retval;
    }

    std::pair<int32_t, int32_t> Position::pathNext(bool bIncrease)
    {
        if (bIncrease && !mMoving)
            return mCurrent;

        if (mIndex != -1 && mPath.size() && mIndex < (int)(mPath.size()))
        {
            auto nextPos = mPath[mIndex];
            if (bIncrease)
            {
                mIndex++;
            }
            return nextPos;
        }
        else
        {
            mIndex = -1;
            mMoving = false;
        }
        return mCurrent;
    }
}
