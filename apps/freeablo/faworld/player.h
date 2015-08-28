#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"

#include <iostream>

namespace FAWorld
{
    class Player: public Actor
    {
        public:
            Player(): Actor("plrgfx/warrior/wls/wlswl.cl2", "plrgfx/warrior/wls/wlsst.cl2", Position(0,0))
            {
                mDestination = mPos.current();
            }

            std::pair<size_t, size_t>& destination()
            {
                return mDestination;
            }

        protected:
            std::pair<size_t, size_t> mDestination;

            friend class boost::serialization::access;

            template<class Archive>
            void save(Archive & ar, const unsigned int version) const
            {
                ar & this->mDestination;
                Actor::save(ar, version);

            }

            template<class Archive>
            void load(Archive & ar, const unsigned int version)
            {
                ar & this->mDestination;
                Actor::load(ar, version);
            }

            BOOST_SERIALIZATION_SPLIT_MEMBER()
    };
}

#endif
