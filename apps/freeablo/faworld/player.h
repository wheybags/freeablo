#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"

#include <iostream>

namespace FAWorld
{
    class Player: public Actor
    {
        public:
            Player(): Actor("plrgfx/warrior/wls/wlswl.cl2") {}
    };
}

#endif
