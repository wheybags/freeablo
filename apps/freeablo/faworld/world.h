#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "actor.h"
#include "player.h"

namespace FARender
{
    class RenderState;
}

namespace FAWorld
{
    class World
    {
        public:
            World();
            void update();
            Player* getPlayer();
            void fillRenderState(FARender::RenderState* state);

        private:
            std::vector<Actor*> mActors;
            Player mPlayer;
    };
}

#endif
