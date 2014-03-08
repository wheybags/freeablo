#include "position.h"

namespace FAWorld
{
    Position::Position(): mDist(0), mCurrent(std::make_pair(0,0)),
        mDirection(-1), mMoving(false) {}

    Position::Position(size_t x, size_t y): mDist(0), mCurrent(std::make_pair(x,y)),
        mDirection(-1), mMoving(false) {}

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

    std::pair<size_t, size_t> Position::current()
    {
        return mCurrent;
    }

    std::pair<size_t, size_t> Position::next()
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
