#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "actor.h"
#include "player.h"

namespace FAWorld
{
    class World
    {
        public:
            World();
            void update();
            Player* getPlayer();

        private:
            std::vector<Actor*> mActors;
            Player mPlayer;
    };
}

#endif
