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

            void setLevel(int32_t levelnum);
            void addNpcs();

            Actor* getActorAt(size_t x, size_t y);

            void clear();

            void deleteActorFromWorld(Actor * dead);

            void update(bool noclip);

            Player* getCurrentPlayer();
            void addPlayer(uint32_t id, Player* player);
            Player* getPlayer(size_t id);
            void setCurrentPlayerId(uint32_t id);
            double secondsToTicks(double seconds);
            size_t getTicksPassed();
            void fillRenderState(FARender::RenderState* state);
            void setStatsObject(CharacterStatsBase *stats);
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

            size_t mTicksSinceLastAnimUpdate=0;
            size_t mTicksPassed=0;
            Player* mCurrentPlayer;
            std::map<uint32_t, Player*> mPlayers;            
            const DiabloExe::DiabloExe& mDiabloExe;
            friend class Actor;
    };
}

#endif
