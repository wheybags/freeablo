#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"

namespace FAWorld
{
    class Player: public Actor
    {
        public:

            Player(): Actor("plrgfx/warrior/wls/wlswl.cl2", "plrgfx/warrior/wls/wlsst.cl2", Position(0,0)) {}


        friend class Inventory;
    };
}

#endif
