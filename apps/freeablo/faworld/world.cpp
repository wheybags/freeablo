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

  bool HoverState::applyIfNeeded(const HoverState& newState) {
    if (*this == newState)
      return false;

    *this = newState;
    return true;
  }

  bool HoverState::operator==(const HoverState& other) const {
    if (type != other.type)
      return false;

    switch (type) {
    case HoverType::actor:
      return actorId == other.actorId;
    }
  }

  bool HoverState::actorHovered(int32_t actorIdArg) {
    HoverState newState (HoverType::actor);
    newState.actorId = actorIdArg;
    return applyIfNeeded (newState);
  }

  bool HoverState::nothingHovered() {
    HoverState newState (HoverType::none);
    return applyIfNeeded (newState);
  }

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
      switch (action) {

      case Engine::MOUSE_RELEASE:
        stopPlayerActions();
        break;
      case Engine::MOUSE_DOWN:
        onMouseDown(mousePosition);
        break;
      case Engine::MOUSE_CLICK:
        onMouseClick(mousePosition);
        break;
      case Engine::MOUSE_MOVE:
        onMouseMove(mousePosition);
        break;
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
            actor->setName (npcs[i]->name);
            townActors.push_back(actor);
        }

        auto tmp = new GameLevel(townLevelBase, 0, townActors);
        mLevels[0] = tmp;


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

    void World::setLevel(size_t level)
    {
        if (level >= mLevels.size() || (mCurrentPlayer->getLevel() && mCurrentPlayer->getLevel()->getLevelIndex() == level))
            return;

        mCurrentPlayer->setLevel(getLevel (level));
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

  void World::setGuiManager(FAGui::GuiManager* guiManager)
  {
    mGuiManager = guiManager;
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

    Render::Tile World::getTileByScreenPos(Engine::Point screenPos) {
      return FARender::Renderer::get()->getTileByScreenPos(screenPos.x, screenPos.y, getCurrentPlayer()->mPos);
    }

    void World::onMouseMove(Engine::Point mousePosition)
    {
      auto tile = getTileByScreenPos(mousePosition);
      auto actor = getActorAt(tile.x, tile.y);
      if (!actor && tile.x < getCurrentLevel ()->width() && tile.y < getCurrentLevel()->height ())
        actor = getActorAt(tile.x + 1, tile.y + 1); // It seems like all actors are 2 "tile" tall for hover and targeting

      if (actor)
          {
            if (m_hoverState.actorHovered (actor->getId ()))
              mGuiManager->setStatusBarText(actor->getName ());

            return;
          }

      if (m_hoverState.nothingHovered ())
        return mGuiManager->setStatusBarText ("");
      // and here we should tecnically run redraw if mHoveredActorId changed.
    }

    void World::onMouseDown(Engine::Point mousePosition)
    {
        auto player = getCurrentPlayer();
        std::pair<int32_t, int32_t>& destination = player->destination();
        auto clickedTile = getTileByScreenPos(mousePosition);
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
