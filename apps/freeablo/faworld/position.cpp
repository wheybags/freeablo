#include "position.h"

namespace FAWorld
{
    Position::Position(): mDist(0), mCurrent(std::make_pair(0,0)),
        mNext(std::make_pair(0,0)) {}

    void Position::update()
    {
        if(mCurrent != mNext)
        {
            mDist += 2;

            if(mDist >= 100)
            {
                mCurrent = mNext;
                mDist = 0;
            }
        }
    }
}
