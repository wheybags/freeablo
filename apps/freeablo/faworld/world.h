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

            GameLevel* getLevel(size_t levelNum);
            void insertLevel(size_t levelNum, GameLevel* level);

            Actor* getActorAt(size_t x, size_t y);

            void update(bool noclip);

            void addCurrentPlayer(Player * player);
            Player* getCurrentPlayer();
            void addPlayer(uint32_t id, Player* player);
            Player* getPlayer(size_t id);
            void setCurrentPlayerId(uint32_t id);

            void fillRenderState(FARender::RenderState* state);
            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated

        private:
            std::map<size_t, std::shared_ptr<GameLevel>> mLevels;

            size_t mTicksPassed=0;
            Player* mCurrentPlayer;
            std::map<uint32_t, Player*> mPlayers;            
            const DiabloExe::DiabloExe& mDiabloExe;
    };
}

#endif
