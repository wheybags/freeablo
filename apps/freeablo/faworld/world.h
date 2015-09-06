#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <utility>

#include "actor.h"
#include "player.h"
#include "gamelevel.h"


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

            static World* get();

            void generateLevels();

            GameLevel* getCurrentLevel();
            size_t getCurrentLevelIndex();

            void setLevel(int32_t levelnum);

            Actor* getActorAt(size_t x, size_t y);

            void update(bool noclip);

            Player* getCurrentPlayer();
            void addPlayer(uint32_t id, Player* player);
            Player* getPlayer(size_t id);
            void setCurrentPlayerId(uint32_t id);

            void fillRenderState(FARender::RenderState* state);
            void setStatsObject(ActorStats *stats);
            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated

        private:
            std::vector<std::shared_ptr<GameLevel>> mLevels;
            GameLevel* mCurrentLevel;
            size_t mCurrentLevelIndex;

            size_t mTicksPassed=0;
            Player* mCurrentPlayer;
            std::map<uint32_t, Player*> mPlayers;            
            const DiabloExe::DiabloExe& mDiabloExe;
            friend class Actor;
    };
}

#endif
