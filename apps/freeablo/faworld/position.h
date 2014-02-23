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
            
            std::pair<size_t, size_t> current(); ///< where we are coming from
            std::pair<size_t, size_t> next(); ///< where we are going to
            
            size_t mDist; ///< percentage of the way there

            int32_t mDirection;
            bool mMoving;
        
        private:
            std::pair<size_t, size_t> mCurrent;

    };
}

#endif
