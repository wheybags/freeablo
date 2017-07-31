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
            std::make_pair(25u, 29u), std::make_pair(75u, 68u), std::map<int32_t, int32_t>(), static_cast<int32_t> (-1), 1);

        auto townLevel = new GameLevel(townLevelBase, 0);
        mLevels[0] = townLevel;

        const std::vector<const DiabloExe::Npc*> npcs = mDiabloExe.getNpcs();
        for (size_t i = 0; i < npcs.size(); i++)
        {
            Actor* actor = new Actor(npcs[i]->celPath, npcs[i]->celPath);
            actor->setCanTalk(true);
            actor->setActorId(npcs[i]->id);
            actor->teleport(townLevel, Position(npcs[i]->x, npcs[i]->y, npcs[i]->rotation));
        }

        for (int32_t i = 1; i < 17; i++)
        {
            mLevels[i] = nullptr; // let's generate levels on demand
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

    void World::setLevel(size_t levelNum)
    {
        if (levelNum >= mLevels.size() || (mCurrentPlayer->getLevel() && mCurrentPlayer->getLevel()->getLevelIndex() == levelNum))
            return;

        auto level = getLevel(levelNum);

        mCurrentPlayer->teleport(level, FAWorld::Position(level->upStairsPos().first, level->upStairsPos().second));
        playLevelMusic(levelNum);

        auto netManager = Engine::NetManager::get();

        if (netManager && !netManager->isServer())
            netManager->sendLevelChangePacket(level->getLevelIndex());
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
        auto p = mLevels.find (level);
        if (p == mLevels.end ())
          return nullptr;
        if (p->second == nullptr)
          {
            p->second = FALevelGen::generate(100, 100, level, mDiabloExe, level - 1, level + 1);
          }
        return p->second;
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
                level->update(noclip);
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
        auto sortedInsertPosIt = std::upper_bound(mPlayers.begin(), mPlayers.end(), player, [](Player* lhs, Player* rhs)
        {
            return lhs->getId() > rhs->getId();
        });

        mPlayers.insert(sortedInsertPosIt, player);
    }

    void World::deregisterPlayer(Player *player)
    {
        mPlayers.erase(std::find(mPlayers.begin(), mPlayers.end(), player));
    }

    const std::vector<Player*>& World::getPlayers()
    {
        return mPlayers;
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
            if (levelPair.second)
            {
                auto actor = levelPair.second->getActorById(id);

                if (actor)
                    return actor;
            }
        }

        return NULL;
    }

    void World::getAllActors(std::vector<Actor*>& actors)
    {
        for (auto pair : mLevels)
        {
            if(pair.second)
                pair.second->getActors(actors);
        }
    }

    Tick World::getCurrentTick()
    {
        return mTicksPassed;
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
            player->teleport(level, Position(level->downStairsPos().first, level->downStairsPos().second));
        else
            player->teleport(level, Position(level->upStairsPos().first, level->upStairsPos().second));
    }

    void World::stopPlayerActions()
    {
        getCurrentPlayer()->isTalking = false;
    }

    void World::onMouseClick(Engine::Point mousePosition)
    {
        auto player = getCurrentPlayer();
        auto clickedTile = FARender::Renderer::get()->getClickedTile(mousePosition.x, mousePosition.y, player->getPos());

        auto level = getCurrentLevel();
        level->activate(clickedTile.x, clickedTile.y);
    }

    void World::onMouseDown(Engine::Point mousePosition)
    {
        auto player = getCurrentPlayer();
        auto clickedTile = FARender::Renderer::get()->getClickedTile(mousePosition.x, mousePosition.y, player->getPos());
        Actor* clickedActor = World::get()->getActorAt(clickedTile.x, clickedTile.y);
        
        if (clickedActor)
            player->actorTarget = clickedActor;
        else
            player->mMoveHandler.setDestination({ clickedTile.x, clickedTile.y });
    }

    Tick World::getTicksInPeriod(float seconds)
    {
        return std::max((Tick)1, (Tick)round(((float)ticksPerSecond) * seconds));
    }

    float World::getSecondsPerTick()
    {
        return 1.0f / ((float)ticksPerSecond);
    }
}
