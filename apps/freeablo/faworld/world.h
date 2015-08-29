#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <utility>

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
            World(const DiabloExe::DiabloExe& exe);
            ~World();

            static World* get();

            void generateLevels();

            Level::Level* getCurrentLevel();
            size_t getCurrentLevelIndex();

            void setLevel(size_t levelnum);
            void addNpcs();

            Actor* getActorAt(size_t x, size_t y);

            void clear();

            void update(bool noclip);
            Player* getPlayer();
            void fillRenderState(FARender::RenderState* state);

            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated

        private:
            void actorMapInsert(Actor* actor); ///< insert actor into 2d map for collision
            void actorMapRemove(Actor* actor);
            void actorMapClear();

            std::vector<Actor*> mActors;

            std::vector<Level::Level> mLevels;
            Level::Level* mCurrentLevel;
            size_t mCurrentLevelIndex;
            std::map<std::pair<size_t, size_t>, Actor*> mActorMap2D;    ///< Contains NULL where no Actor is present.
                                                                        ///< Where an actor straddles two squares, they shall be placed in both.
            Player* mPlayer;
            size_t mTicksSinceLastAnimUpdate;

            const DiabloExe::DiabloExe& mDiabloExe;
    };
}

#endif
