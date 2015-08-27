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

        private:
            std::pair<size_t, size_t> mDestination;
    };
}

#endif
