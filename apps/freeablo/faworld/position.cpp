#include "position.h"

namespace FAWorld
{
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
