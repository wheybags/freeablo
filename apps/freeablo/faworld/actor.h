#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"

namespace FAWorld
{
    class Actor
    {
        public:
            void update();
            Position mPos;
    };
}

#endif
