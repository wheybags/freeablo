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
            std::pair<int32_t, int32_t> next() const; ///< where we are going to
            std::pair<int32_t, int32_t> pathNext(bool bIncrease ); ///< get the next find path way node.

            int32_t mDist; ///< percentage of the way there
            int32_t mDirection;
            std::vector<std::pair<int32_t, int32_t>> mPath;  ///< find path result
            std::pair<int32_t, int32_t> mGoal;  ///< the movement goal point. it maybe changed by findPath,otherwise it maybe 0 if findpath failed. It's necessary and different from World::mDestination.
            int mIndex;///< index
            bool mMoving;
            double distanceFrom(const Position &B);

        private:
            std::pair<int32_t, int32_t> mCurrent;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                UNUSED_PARAM(version);

                ar << mDirection;
                ar << mCurrent;
            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                UNUSED_PARAM(version);

                ar >> mDirection;
                ar >> mCurrent;
                mMoving = false;
                mDist = 0;
            }


            friend class boost::serialization::access;

            BOOST_SERIALIZATION_SPLIT_MEMBER()

            template<class Stream>
            Serial::Error::Error faSerial(Stream& stream)
            {
                serialise_int(stream, 0, 100, mDist);
                serialise_int(stream, 0, 7, mDirection);
                serialise_bool(stream, mMoving);
                serialise_int32(stream, mCurrent.first);
                serialise_int32(stream, mCurrent.second);

                return Serial::Error::Success;
            }

            friend class Serial::WriteBitStream;
            friend class Serial::ReadBitStream;
    };
}

#endif
