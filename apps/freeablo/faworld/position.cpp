#include "position.h"

#include "world.h"

namespace FAWorld
{
    Position::Position() : mDist(0), mDirection(0), mMoving(false),
        mCurrent(std::make_pair(0, 0)) {}

    Position::Position(int32_t x, int32_t y) : mDist(0), mDirection(0), mMoving(false),
        mCurrent(std::make_pair(x, y)) {}

    Position::Position(int32_t x, int32_t y, int32_t direction) : mDist(0), mDirection(direction), mMoving(false),
        mCurrent(std::make_pair(x, y)) {}

    void Position::update()
    {
        if (mMoving)
        {
            mDist += static_cast<int32_t> (FAWorld::World::getSecondsPerTick() * 250);

            if (mDist >= 100)
            {
                mCurrent = next();
                mDist = 0;
            }
        }
    }

    int32_t Position::getDirection() const
    {
        return mDirection;
    }

    void Position::setDirection(int32_t mDirection)
    {
        if (mDirection >= 0)
            this->mDirection = mDirection;
    }

    std::pair<int32_t, int32_t> Position::current() const
    {
        return mCurrent;
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

    template<class Stream>
    Serial::Error::Error Position::faSerial(Stream& stream)
    {
        serialise_int(stream, 0, 100, mDist);
        serialise_int(stream, 0, 7, mDirection);
        serialise_bool(stream, mMoving);
        serialise_int32(stream, mCurrent.first);
        serialise_int32(stream, mCurrent.second);

        return Serial::Error::Success;
    }

    FA_SERIAL_TEMPLATE_INSTANTIATE(Position);
}
