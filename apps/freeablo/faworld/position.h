#ifndef POSITION_H
#define POSITION_H

#include <utility>
#include <stdlib.h>

namespace FAWorld
{
    class Position
    {
        public:
            Position();

            void update(); ///< advances towards mNext

            std::pair<size_t, size_t> mCurrent; ///< where we are coming from
            std::pair<size_t, size_t> mNext; ///< where we are going to

            size_t mDist; ///< percentage of the way there
    };
}

#endif
