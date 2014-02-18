#ifndef ACTOR_H
#define ACTOR_H

#include "position.h"

#include "../farender/renderer.h"

namespace FAWorld
{
    class Actor
    {
        public:
            Actor(const std::string& walkAnimPath):
                walkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)) {}
            void update();
            Position mPos;
        //private: //TODO: fix this
            FARender::FASpriteGroup walkAnim;
    };
}

#endif
