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
#include "itemmap.h"

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

    Render::Tile World::getTileByScreenPos(Misc::Point screenPos) {
       return FARender::Renderer::get()->getTileByScreenPos(screenPos.x, screenPos.y, getCurrentPlayer()->getPos());
     }

    Actor *World::targetedActor(Misc::Point screenPosition)
    {
       auto actorStayingAt = [this](int32_t x, int32_t y) -> Actor*
       {
         if (x >= static_cast<int32_t> (getCurrentLevel ()->width()) || y >= static_cast<int32_t> (getCurrentLevel ()->height()))
           return nullptr;

         auto actor = getActorAt (x, y);
         if (actor && !actor->isDead() && actor != getCurrentPlayer()) return actor;

         return nullptr;
       };
       auto tile = getTileByScreenPos(screenPosition);
       // actors could be hovered/targeted by hexagonal pattern consisiting of two tiles on top of each other + halves of two adjacent tiles
       // the same logic seems to apply ot other tall objects
       if (auto actor = actorStayingAt(tile.x, tile.y)) return actor;
       if (auto actor = actorStayingAt(tile.x + 1, tile.y + 1)) return actor;
       if (tile.half == Render::TileHalf::right)
         if (auto actor = actorStayingAt(tile.x + 1, tile.y))
           return actor;
       if (tile.half == Render::TileHalf::left)
         if (auto actor = actorStayingAt(tile.x, tile.y + 1))
           return actor;
       return nullptr;
     }

    void World::updateHover(const Misc::Point& mousePosition) {
        auto nothingHovered = [&]
        {
            if (getHoverState().setNothingHovered ())
                return mGuiManager->setDescription ("");
        };

        auto &cursorItem = mCurrentPlayer->getInventory ().getItemAt(MakeEquipTarget<Item::eqCURSOR> ());
        if (!cursorItem.isEmpty())
          {
              mGuiManager->setDescription(cursorItem.getName());
              return nothingHovered ();
          }

        auto actor = targetedActor (mousePosition);
        if (actor != nullptr)
        {
            if (getHoverState().setActorHovered (actor->getId ()))
                mGuiManager->setDescription(actor->getName ());

            return;
        }
        if (auto item = targetedItem (mousePosition))
        {
          if (getHoverState().setItemHovered (item->getTile ()))
            mGuiManager->setDescription(item->item ().getName());

          return;
        }

        return nothingHovered ();
    }

    void World::onMouseMove(const Misc::Point &/*mousePosition*/)
    {
        return;
    }

    void World::notify(Engine::MouseInputAction action, Misc::Point mousePosition)
    {
        switch (action) {
            case Engine::MOUSE_RELEASE: return onMouseRelease();
            case Engine::MOUSE_DOWN: return onMouseDown(mousePosition);
            case Engine::MOUSE_CLICK: return onMouseClick(mousePosition);
            case Engine::MOUSE_MOVE: return onMouseMove(mousePosition);
            default: ;
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
            if (auto id = npcs[i]->animationSequenceId)
                actor->setIdleAnimSequence (mDiabloExe.getTownerAnimation()[*id]);
            actor->setCanTalk(true);
            actor->setActorId(npcs[i]->id);
            actor->setName (npcs[i]->name);
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

    HoverState& World::getHoverState()
    {
        return getCurrentLevel()->getHoverState();
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

        {
            if (!nk_item_is_any_active(FARender::Renderer::get()->getNuklearContext())) {
                // we need update hover not only on mouse move because viewport may move without mouse being moved
                int x, y;
                SDL_GetMouseState(&x,&y);
                updateHover(Misc::Point {x, y});
            }
            else if (getHoverState().setNothingHovered ())
              return mGuiManager->setDescription ("");
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
            getCurrentLevel()->fillRenderState(state, getCurrentPlayer());
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

    void World::onMouseRelease()
    {
        targetLock = false;
        simpleMove = false;
        getCurrentPlayer()->isTalking = false;
    }

    void World::onMouseClick(Misc::Point mousePosition)
    {
        auto player = getCurrentPlayer();
        auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());

        auto level = getCurrentLevel();
        level->activate(clickedTile.x, clickedTile.y);
    }

    PlacedItemData *World::targetedItem(Misc::Point screenPosition)
    {
        auto tile = getTileByScreenPos(screenPosition);
        return getCurrentLevel ()->getItemMap ().getItemAt ({tile.x, tile.y});
    }

    void World::onMouseDown(Misc::Point mousePosition)
    {
        auto player = getCurrentPlayer();
        auto &inv = player->getInventory ();
        auto clickedTile = FARender::Renderer::get()->getTileByScreenPos(mousePosition.x, mousePosition.y, player->getPos());

        bool targetWasLocked = targetLock; // better solution assign targetLock true at something like SCOPE_EXIT macro
        targetLock = true;

        if (!targetWasLocked)
        {
            auto cursorItem = inv.getItemAt(MakeEquipTarget<Item::eqCURSOR> ());
            if (!cursorItem.isEmpty()) {
                // What happens here is not actually true to original game but
                // It's a fair way to emulate it. Current data is that in all instances except interaction with inventory
                // cursor has topleft as it's hotspot even when cursor is item. Other 2 instances actually:
                // - dropping items
                // - moving cursor outside the screen / window
                // This shift by half cursor size emulates behavior during dropping items. And the only erroneous
                // part of behavior now can be spotted by moving cursor outside the window which is not so significant.
                // To emulate it totally true to original game we need to heavily hack interaction with inventory
                // which is possible
                auto clickedTileShifted = getTileByScreenPos(mousePosition - FARender::Renderer::get()->cursorSize() / 2);
                if (player->dropItem ({clickedTileShifted.x, clickedTileShifted.y}))
                    {
                        mGuiManager->clearDescription();
                    }
                return;
            }
        }

       if (!targetWasLocked)
           {
               Actor* clickedActor = targetedActor (mousePosition);
               if (clickedActor)
                   {
                       player->setTarget (clickedActor);
                       return;
                   }

              if (auto item = targetedItem(mousePosition)) {
                player->setTarget (ItemTarget {mGuiManager->isInventoryShown() ?
                    ItemTarget::ActionType::toCursor :
                    ItemTarget::ActionType::autoEquip, item});
                return;
              }
           }

       if (!targetWasLocked || simpleMove)
          {
              player->setTarget (boost::blank{});
              player->mMoveHandler.setDestination({ clickedTile.x, clickedTile.y });
              simpleMove = true;
          }
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
