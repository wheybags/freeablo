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
            ~World();

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

            void registerPlayer(Player* player);
            void deregisterPlayer(Player* player);

            void fillRenderState(FARender::RenderState* state);
            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated

            Actor* getActorById(int32_t id);

            void getAllActors(std::vector<Actor*>& actors);

        private:
            std::map<size_t, GameLevel*> mLevels;

            size_t mTicksPassed=0;
            Player* mCurrentPlayer;
            std::vector<Player*> mPlayers;
            const DiabloExe::DiabloExe& mDiabloExe;
    };
}

#endif
