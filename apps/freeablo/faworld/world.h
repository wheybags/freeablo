#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <utility>

#include "actor.h"
#include "player.h"
#include "gamelevel.h"
#include "../engine/inputobserverinterface.h"


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
    class World : public Engine::KeyboardInputObserverInterface, public Engine::MouseInputObserverInterface
    {
        public:
            World(const DiabloExe::DiabloExe& exe);
            ~World();

            static World* get();
            void notify(Engine::KeyboardInputAction action);
            void notify(Engine::MouseInputAction action, Engine::Point mousePosition);
            void generateLevels();
            GameLevel* getCurrentLevel();
            size_t getCurrentLevelIndex();

            void setLevel(size_t level);
            GameLevel* getLevel(size_t level);
            void insertLevel(size_t level, GameLevel* gameLevel);

            Actor* getActorAt(size_t x, size_t y);

            void update(bool noclip);

            void addCurrentPlayer(Player * player);
            Player* getCurrentPlayer();

            void registerPlayer(Player* player);
            void deregisterPlayer(Player* player);

            void fillRenderState(FARender::RenderState* state);
            
            static const size_t ticksPerSecond = 125; ///< number of times per second that game state will be updated
            static size_t getTicksInPeriod(float seconds);
            static float World::getSecondsPerTick();

            Actor* getActorById(int32_t id);

            void getAllActors(std::vector<Actor*>& actors);

        private:
            void playLevelMusic(size_t level);
            void changeLevel(bool up);
            void stopPlayerActions();
            void onMouseClick(Engine::Point mousePosition);
            void onMouseDown(Engine::Point mousePosition);

            std::map<size_t, GameLevel*> mLevels;
            size_t mTicksPassed = 0;
            Player* mCurrentPlayer;
            std::vector<Player*> mPlayers;
            const DiabloExe::DiabloExe& mDiabloExe;
            std::pair<int32_t, int32_t> mDestination;
    };
}

#endif
