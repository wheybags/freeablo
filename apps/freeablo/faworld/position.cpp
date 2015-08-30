#include "position.h"

namespace FAWorld
{
    Position::Position(): mDist(0), mDirection(0),
        mMoving(false), mCurrent(std::make_pair(0,0)) {}

    Position::Position(size_t x, size_t y): mDist(0), mDirection(0), 
        mMoving(false), mCurrent(std::make_pair(x,y)) {}

    Position::Position(size_t x, size_t y, size_t direction): mDist(0), 
        mDirection(direction), mMoving(false),
        mCurrent(std::make_pair(x,y)) {}

    void Position::update()
    {
        if(mMoving)
        {
            mDist += 2;

            if(mDist >= 100)
            {
                mCurrent = next();
                mDist = 0;
            }
        }
    }

    std::pair<size_t, size_t> Position::current() const
    {
        return mCurrent;
    }

    double Position::distanceFrom(Position B)
    {
        return sqrt(pow(mCurrent.first - B.mCurrent.first, 2) + pow(mCurrent.second - B.mCurrent.second, 2));

    }

    std::pair<size_t, size_t> Position::next() const
    {
        if(!mMoving)
            return mCurrent;
        
        std::pair<size_t, size_t> retval = mCurrent;

        switch(mDirection)
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
}
