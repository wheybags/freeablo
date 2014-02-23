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
                mWalkAnim(FARender::Renderer::get()->loadImage(walkAnimPath)), mFrame(0) {}
            void update();
            Position mPos;
        //private: //TODO: fix this
            FARender::FASpriteGroup mWalkAnim;
            size_t mFrame;
    };
}

#endif
