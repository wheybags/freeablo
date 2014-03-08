#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "actor.h"
#include "player.h"

#include <level/level.h>

namespace FARender
{
    class RenderState;
}

namespace DiabloExe
{
    class DiabloExe;
}

namespace FAWorld
{
    class World
    {
        public:
            World();
            ~World();

            void setLevel(const Level::Level& level, DiabloExe::DiabloExe& exe);
            void update();
            Player* getPlayer();
            void fillRenderState(FARender::RenderState* state);

            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated

        private:
            std::vector<Actor*> mActors;
            Player* mPlayer;
            size_t mTicksSinceLastAnimUpdate;
    };
}

#endif
