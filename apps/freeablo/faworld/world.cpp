#include "world.h"

#include <tuple>
#include <diabloexe/diabloexe.h>
#include "../farender/renderer.h"
#include "../falevelgen/levelgen.h"
#include "../faaudio/audiomanager.h"
#include "../engine/threadmanager.h"
#include "actorstats.h"
#include "monster.h"

namespace FAWorld
{
    World* singletonInstance = nullptr;

    World::World(const DiabloExe::DiabloExe& exe) : mDiabloExe(exe)
    {
        assert(singletonInstance == nullptr);
        singletonInstance = this;

        mCurrentPlayer = new Player();

        mCurrentLevel = nullptr;
    }

    void World::setStatsObject(ActorStats *stats)
    {
        mCurrentPlayer->setStats(stats);
    }

    World* World::get()
    {
        return singletonInstance;
    }

    void World::generateLevels()
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        Level::Level townLevelBase( Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til",
                                    "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel",
                                    std::make_pair(25,29), std::make_pair(75,68), std::map<size_t, size_t>(), -1, 1);

        std::vector<Actor*> townActors;

        const std::vector<const DiabloExe::Npc*> npcs = mDiabloExe.getNpcs();

        for(size_t i = 0; i < npcs.size(); i++)
        {
            Actor* actor = new Actor(npcs[i]->celPath, npcs[i]->celPath, Position(npcs[i]->x, npcs[i]->y, npcs[i]->rotation));
            townActors.push_back(actor);
        }

        mLevels.push_back(std::shared_ptr<GameLevel>(new GameLevel(townLevelBase, townActors)));


        for(int32_t i = 1; i < 13; i++)
        {
            mLevels.push_back(std::move(FALevelGen::generate(100, 100, i, mDiabloExe, i-1, i+1)));
        }
    }

    GameLevel* World::getCurrentLevel()
    {
        return mCurrentLevel;
    }

    size_t World::getCurrentLevelIndex()
    {
        return mCurrentLevelIndex;
    }
    
    void World::setLevel(int32_t levelNum)
    {
        if(levelNum != 0)
            mCurrentPlayer->mInDungeon=true;
        else
            mCurrentPlayer->mInDungeon=false;

        if(levelNum >= (int32_t)mLevels.size() || levelNum < 0)
            return;

        if(mCurrentLevel)
            mCurrentLevel->removeActor(mCurrentPlayer);

        mCurrentLevel = mLevels[levelNum].get();
        mCurrentLevelIndex = levelNum;

        mCurrentLevel->addPlayer(mCurrentPlayer);

        FAAudio::AudioManager::playLevelMusic(levelNum, *Engine::ThreadManager::get());
    }

    Actor* World::getActorAt(size_t x, size_t y)
    {
        return getCurrentLevel()->getActorAt(x, y);
    }

    void World::update(bool noclip)
    {
        mTicksPassed++;
                        
        if(getCurrentLevel())
            getCurrentLevel()->update(noclip, mTicksPassed);
    }

    Player* World::getCurrentPlayer()
    {
        return mCurrentPlayer;
    }

    Player* World::getPlayer(size_t id)
    {
        if(mPlayers.find(id) != mPlayers.end())
            return mPlayers[id];
        else
            return NULL;
    }

    void World::addPlayer(uint32_t id, Player *player)
    {
        mPlayers[id] = player;

        getCurrentLevel()->addPlayer(player);
    }

    void World::setCurrentPlayerId(uint32_t id)
    {
        mPlayers[id] = mCurrentPlayer;
    }

    void World::fillRenderState(FARender::RenderState* state)
    {
        getCurrentLevel()->fillRenderState(state);
    }
}
