#include <tuple>
#include <iostream>
#include <algorithm>

#include <diabloexe/diabloexe.h>
#include "../farender/renderer.h"
#include "../falevelgen/levelgen.h"
#include "../faaudio/audiomanager.h"
#include "../engine/threadmanager.h"
#include "../engine/enginemain.h"
#include "actorstats.h"
#include "monster.h"
#include "world.h"
#include "actor.h"
#include "player.h"
#include "gamelevel.h"
#include "findpath.h"

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
        for (auto& pair : mLevels)
            delete pair.second;
    }

    World* World::get()
    {
        return singletonInstance;
    }

    void World::notify(Engine::KeyboardInputAction action)
    {
        if (action == Engine::CHANGE_LEVEL_UP || action == Engine::CHANGE_LEVEL_DOWN)
        {
            changeLevel(action == Engine::CHANGE_LEVEL_UP);
        }
    }

    void World::notify(Engine::MouseInputAction action, Engine::Point mousePosition)
    {
        if (action == Engine::MOUSE_RELEASE)
        {
            stopPlayerActions();
        }
        else if (action == Engine::MOUSE_CLICK)
        {
            onMouseClick(mousePosition);
        }
        else if (action == Engine::MOUSE_DOWN)
        {
            onMouseDown(mousePosition);
        }
    }

    void World::generateLevels()
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        Level::Level townLevelBase(Level::Dun::getTown(sector1, sector2, sector3, sector4), "levels/towndata/town.til",
            "levels/towndata/town.min", "levels/towndata/town.sol", "levels/towndata/town.cel",
            std::make_pair(25, 29), std::make_pair(75, 68), std::map<size_t, size_t>(), -1, 1);

        std::vector<Actor*> townActors;

        const std::vector<const DiabloExe::Npc*> npcs = mDiabloExe.getNpcs();

        for (size_t i = 0; i < npcs.size(); i++)
        {
            Actor* actor = new Actor(npcs[i]->celPath, npcs[i]->celPath, Position(npcs[i]->x, npcs[i]->y, npcs[i]->rotation));
            actor->setCanTalk(true);
            actor->setActorId(npcs[i]->id);
            townActors.push_back(actor);
        }

        auto tmp = new GameLevel(townLevelBase, 0, townActors);
        mLevels[0] = tmp;


        for (int32_t i = 1; i < 17; i++)
        {
            mLevels[i] = FALevelGen::generate(100, 100, i, mDiabloExe, i - 1, i + 1);
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

    void World::setLevel(size_t level)
    {
        if (level >= mLevels.size() || (mCurrentPlayer->getLevel() && mCurrentPlayer->getLevel()->getLevelIndex() == level))
            return;

        mCurrentPlayer->setLevel(mLevels[level]);
        playLevelMusic(level);
    }

    void World::playLevelMusic(size_t level)
    {
        auto threadManager = Engine::ThreadManager::get();
        switch (level)
        {
        case 0:
        {
            threadManager->playMusic("music/dtowne.wav");
            break;
        }
        case 1: case 2: case 3: case 4:
        {
            threadManager->playMusic("music/dlvla.wav");
            break;
        }
        case 5: case 6: case 7: case 8:
        {
            threadManager->playMusic("music/dlvlb.wav");
            break;
        }
        case 9: case 10: case 11: case 12:
        {
            threadManager->playMusic("music/dlvlc.wav");
            break;
        }
        case 13: case 14: case 15: case 16:
        {
            threadManager->playMusic("music/dlvld.wav");
            break;
        }
        default:
            std::cout << "Wrong level " << level << std::endl;
            break;
        }
    }

    GameLevel* World::getLevel(size_t level)
    {
        return mLevels[level];
    }

    void World::insertLevel(size_t level, GameLevel *gameLevel)
    {
        mLevels[level] = gameLevel;
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
        for (auto& player : mPlayers)
        {
            GameLevel* level = player->getLevel();

            if (level && !done.count(level))
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
        if (getCurrentLevel())
            getCurrentLevel()->fillRenderState(state);
    }

    Actor* World::getActorById(int32_t id)
    {
        for (auto levelPair : mLevels)
        {
            auto actor = levelPair.second->getActorById(id);

            if (actor)
                return actor;
        }

        return NULL;
    }

    void World::getAllActors(std::vector<Actor*>& actors)
    {
        for (auto pair : mLevels)
            pair.second->getActors(actors);
    }

    void World::changeLevel(bool up)
    {
        size_t nextLevelIndex;
        if (up)
            nextLevelIndex = getCurrentLevel()->getPreviousLevel();
        else
            nextLevelIndex = getCurrentLevel()->getNextLevel();

        setLevel(nextLevelIndex);

        GameLevel* level = getCurrentLevel();
        Player* player = getCurrentPlayer();

        if (up)
            player->mPos = Position(level->downStairsPos().first, level->downStairsPos().second);
        else
            player->mPos = Position(level->upStairsPos().first, level->upStairsPos().second);


        player->destination() = player->mPos.current();
    }

    void World::stopPlayerActions()
    {
        getCurrentPlayer()->isTalking = false;
    }

    void World::onMouseClick(Engine::Point mousePosition)
    {
        UNUSED_PARAM(mousePosition);
        auto level = getCurrentLevel();
        level->activate(mDestination.first, mDestination.second);
    }

    void World::onMouseDown(Engine::Point mousePosition)
    {
        auto player = getCurrentPlayer();
        std::pair<int32_t, int32_t>& destination = player->destination();
        auto clickedTile = FARender::Renderer::get()->getClickedTile(mousePosition.x, mousePosition.y, player->mPos);
        destination = {clickedTile.x, clickedTile.y};
        mDestination = player->mPos.mGoal = destination; //update it.
    }

    size_t World::getTicksInPeriod(float seconds)
    {
        return std::max((size_t)1, (size_t)round(((float)ticksPerSecond) * seconds));
    }

    float World::getSecondsPerTick()
    {
        return 1.0f / ((float)ticksPerSecond);
    }
}
