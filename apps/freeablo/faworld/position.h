#ifndef POSITION_H
#define POSITION_H

#include <utility>
#include <stdlib.h>
#include <stdint.h>
#include <cmath>
#include "../fasavegame/savegame.h"
#include "netobject.h"

#include <misc/misc.h>
#include <serial/bitstream.h>

namespace FAWorld
{
    class Position
    {
        public:
            Position();
            Position(int32_t x, int32_t y);
            Position(int32_t x, int32_t y, int32_t direction);

            void update(); ///< advances towards mNext
            std::pair<int32_t, int32_t> current() const; ///< where we are coming from
            bool isNear(const Position& other);
            std::pair<int32_t, int32_t> next() const; ///< where we are going to

            int32_t getDirection() const;
            void setDirection(int32_t mDirection);

            double distanceFrom(Position B);

            template<class Stream>
            Serial::Error::Error faSerial(Stream& stream);

            bool isMoving() const { return mMoving; }
            int32_t getDist() const { return mDist; }

            void stop()
            {
                mDist = 0;
                mMoving = false;
            }

            void start()
            {
                mDist = 0;
                mMoving = true;
            }

        private:
            int32_t mDist; ///< percentage of the way there
            int32_t mDirection;
            bool mMoving;
            std::pair<int32_t, int32_t> mCurrent;
    };
}

#endif
