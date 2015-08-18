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

            void setLevel(Level::Level& level, const DiabloExe::DiabloExe& exe);
            void addNpcs(const DiabloExe::DiabloExe& exe);

            Actor* getActorAt(size_t x, size_t y);

            void clear();

            void update();
            Player* getPlayer();
            void fillRenderState(FARender::RenderState* state);
            void setStatsObject(ActorStats *stats);
            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated

        private:
            void actorMapInsert(Actor* actor); ///< insert actor into 2d map for collision
            void actorMapClear();

            std::vector<Actor*> mActors;

            Level::Level* mLevel;
            std::vector<Actor*> mActorMap2D; ///< 2d array of same size as current level, containing all actors at their
                                             ///< positions. Contains NULL where no Actor is present.
                                             ///< Where an actor straddles two squares, they shall be placed in both.
            Player* mPlayer;
            size_t mTicksSinceLastAnimUpdate;
    };
}

#endif
