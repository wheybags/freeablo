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
    }

    World::~World()
    {
        for(auto& pair : mLevels)
            delete pair.second;
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

        auto tmp = new GameLevel(townLevelBase, 0, townActors);
        mLevels[0] = tmp;


        for(int32_t i = 1; i < 13; i++)
        {
            mLevels[i] = FALevelGen::generate(100, 100, i, mDiabloExe, i-1, i+1);
        }
    }

    GameLevel* World::getCurrentLevel()
    {
        return mCurrentPlayer->getLevel();
    }

    size_t World::getCurrentLevelIndex()
    {
        return mCurrentPlayer->getLevel()->getLevelIndex();
    }
    
    void World::setLevel(int32_t levelNum)
    {
        if(levelNum >= (int32_t)mLevels.size() || levelNum < 0 || (mCurrentPlayer->getLevel() && (int32_t)mCurrentPlayer->getLevel()->getLevelIndex() == levelNum))
            return;

        mCurrentPlayer->setLevel(mLevels[levelNum]);

        FAAudio::AudioManager::playLevelMusic(levelNum, *Engine::ThreadManager::get());
    }

    GameLevel* World::getLevel(size_t levelNum)
    {
        return mLevels[levelNum];
    }

    void World::insertLevel(size_t levelNum, GameLevel *level)
    {
        mLevels[levelNum] = level;
    }

    Actor* World::getActorAt(size_t x, size_t y)
    {
        return getCurrentLevel()->getActorAt(x, y);
    }

    void World::update(bool noclip)
    {
        mTicksPassed++;
        
        std::set<GameLevel*> done;
        
        // only update levels that have players on them
        for(auto& player : mPlayers)
        {
            GameLevel* level = player->getLevel();
            
            if(level && !done.count(level))
            {
                done.insert(level);
                level->update(noclip, mTicksPassed);
            }
        }
    }

    Player* World::getCurrentPlayer()
    {
        return mCurrentPlayer;
    }

    void World::addCurrentPlayer(Player * player)
    {
        mCurrentPlayer = player;
    }

    void World::registerPlayer(Player *player)
    {
        mPlayers.push_back(player);
    }

    void World::deregisterPlayer(Player *player)
    {
        mPlayers.erase(std::find(mPlayers.begin(), mPlayers.end(), player));
    }

    void World::fillRenderState(FARender::RenderState* state)
    {
        if(getCurrentLevel())
            getCurrentLevel()->fillRenderState(state);
    }

    Actor* World::getActorById(size_t id)
    {
        for(auto levelPair : mLevels)
        {
            auto actor = levelPair.second->getActorById(id);

            if(actor)
                return actor;
        }

        return NULL;
    }
}
