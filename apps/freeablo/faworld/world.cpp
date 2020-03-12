#include "world.h"
#include "../engine/enginemain.h"
#include "../engine/net/multiplayerinterface.h"
#include "../engine/threadmanager.h"
#include "../faaudio/audiomanager.h"
#include "../fagui/dialogmanager.h"
#include "../fagui/guimanager.h"
#include "../falevelgen/levelgen.h"
#include "../farender/renderer.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include "actor/attackstate.h"
#include "actor/basestate.h"
#include "actorstats.h"
#include "diabloexe/npc.h"
#include "equiptarget.h"
#include "findpath.h"
#include "gamelevel.h"
#include "itemmap.h"
#include "player.h"
#include "playerbehaviour.h"
#include "storedata.h"
#include <algorithm>
#include <diabloexe/diabloexe.h>
#include <iostream>
#include <misc/assert.h>
#include <serial/textstream.h>
#include <tuple>

namespace FAWorld
{
    World::World(const DiabloExe::DiabloExe& exe, uint32_t seed)
        : mDiabloExe(exe), mRng(new Random::RngMersenneTwister(seed)),
          mLevelRng(new Random::RngMersenneTwister(uint32_t(mRng->randomInRange(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max())))),
          mItemFactory(std::make_unique<ItemFactory>(exe, *mRng.get())), mStoreData(std::make_unique<StoreData>(*mItemFactory))
    {
        this->setupObjectIdMappers();

        if (mDiabloExe.isLoaded())
            regenerateStoreItems();
    }

    void World::regenerateStoreItems() { mStoreData->regenerateGriswoldBasicItems(10 /*placeholder*/, *mRng.get()); }

    void World::load(FASaveGame::GameLoader& loader)
    {
        // reconstruct in-place to reset to default state
        {
            const DiabloExe::DiabloExe& tmp = mDiabloExe;
            this->~World();
            new (this) World(tmp, 0U);
        }

        loader.currentlyLoadingWorld = this;

        mRng->load(loader);
        mLevelRng->load(loader);
        this->mTicksPassed = loader.load<Tick>();
        uint32_t numLevels = loader.load<uint32_t>();

        for (uint32_t i = 0; i < numLevels; i++)
        {
            int32_t levelIndex = loader.load<int32_t>();

            bool hasThisLevel = loader.load<bool>();
            GameLevel* level = nullptr;

            if (hasThisLevel)
                level = new GameLevel(*this, loader);

            mLevels[levelIndex] = level;
        }

        mNextId = loader.load<int32_t>();
        mNextPlayerClass = PlayerClass(loader.load<uint8_t>());
        mStoreData->load(loader);

        loader.runFunctionsToRunAtEnd();

        loader.currentlyLoadingWorld = nullptr;
    }

    void World::save(FASaveGame::GameSaver& saver) const
    {
        mRng->save(saver);
        mLevelRng->save(saver);
        saver.save(this->mTicksPassed);
        uint32_t numLevels = mLevels.size();
        saver.save(numLevels);

        for (auto& pair : mLevels)
        {
            saver.save(pair.first);

            bool hasThisLevel = pair.second != nullptr;
            saver.save(hasThisLevel);

            if (hasThisLevel)
                pair.second->save(saver);
        }

        saver.save(mNextId);
        saver.save(uint8_t(mNextPlayerClass));
        mStoreData->save(saver);
    }

    void World::setupObjectIdMappers()
    {
        mObjectIdMapper.addClass(Actor::typeId, [&](FASaveGame::GameLoader& loader) { return new Actor(*this, loader); });
        mObjectIdMapper.addClass(Player::typeId, [&](FASaveGame::GameLoader& loader) { return new Player(*this, loader); });
        mObjectIdMapper.addClass(Monster::typeId, [&](FASaveGame::GameLoader& loader) { return new Monster(*this, loader); });

        mObjectIdMapper.addClass(NullBehaviour::typeId, [](FASaveGame::GameLoader&) { return new NullBehaviour(); });
        mObjectIdMapper.addClass(BasicMonsterBehaviour::typeId, [](FASaveGame::GameLoader& loader) { return new BasicMonsterBehaviour(loader); });
        mObjectIdMapper.addClass(PlayerBehaviour::typeId, [](FASaveGame::GameLoader& loader) { return new PlayerBehaviour(loader); });

        mObjectIdMapper.addClass(ActorState::MeleeAttackState::typeId, [](FASaveGame::GameLoader& loader) { return new ActorState::MeleeAttackState(loader); });
        mObjectIdMapper.addClass(ActorState::RangedAttackState::typeId,
                                 [](FASaveGame::GameLoader& loader) { return new ActorState::RangedAttackState(loader); });
        mObjectIdMapper.addClass(ActorState::SpellAttackState::typeId, [](FASaveGame::GameLoader& loader) { return new ActorState::SpellAttackState(loader); });
        mObjectIdMapper.addClass(ActorState::BaseState::typeId, [](FASaveGame::GameLoader&) { return new ActorState::BaseState(); });
    }

    World::~World()
    {
        for (auto& pair : mLevels)
            delete pair.second;
    }

    void World::setFirstPlayerAsCurrent()
    {
        release_assert(!mPlayers.empty());
        this->mCurrentPlayer = mPlayers[0];
    }

    Render::Tile World::getTileByScreenPos(Misc::Point screenPos)
    {
        return FARender::Renderer::get()->getTileByScreenPos(screenPos.x, screenPos.y, getCurrentPlayer()->getPos());
    }

    Actor* World::targetedActor(Misc::Point screenPosition)
    {
        auto actorStayingAt = [this](int32_t x, int32_t y) -> Actor* {
            if (x >= static_cast<int32_t>(getCurrentLevel()->width()) || y >= static_cast<int32_t>(getCurrentLevel()->height()))
                return nullptr;

            auto actor = getActorAt(Misc::Point(x, y));
            if (actor && !actor->isDead() && actor != getCurrentPlayer())
                return actor;

            return nullptr;
        };
        auto tile = getTileByScreenPos(screenPosition);
        // actors could be hovered/targeted by hexagonal pattern consisiting of two tiles on top of each other + halves of two adjacent tiles
        // the same logic seems to apply ot other tall objects
        if (auto actor = actorStayingAt(tile.pos.x, tile.pos.y))
            return actor;
        if (auto actor = actorStayingAt(tile.pos.x + 1, tile.pos.y + 1))
            return actor;
        if (tile.half == Render::TileHalf::right)
            if (auto actor = actorStayingAt(tile.pos.x + 1, tile.pos.y))
                return actor;
        if (tile.half == Render::TileHalf::left)
            if (auto actor = actorStayingAt(tile.pos.x, tile.pos.y + 1))
                return actor;
        return nullptr;
    }

    void World::blockInput()
    {
        if (getCurrentPlayer())
            getCurrentPlayer()->getPlayerBehaviour()->blockInput();
    }

    void World::unblockInput()
    {
        if (getCurrentPlayer())
            getCurrentPlayer()->getPlayerBehaviour()->unblockInput();
    }

    const ItemFactory& World::getItemFactory() const { return *mItemFactory; }

    void World::generateLevels()
    {
        Level::Dun sector1("levels/towndata/sector1s.dun");
        Level::Dun sector2("levels/towndata/sector2s.dun");
        Level::Dun sector3("levels/towndata/sector3s.dun");
        Level::Dun sector4("levels/towndata/sector4s.dun");

        // Map from tileset frame number to special cel frame number.
        // Special cel images are mostly used for arches / open doors.
        // The town special cel images overlay on trees, although do
        // not seem to make any difference to the final output.
        // TODO: load specialCelMap from file.
        std::map<int32_t, int32_t> specialCelMap = {
            {357, 1}, {128, 5}, {129, 6}, {127, 7}, {116, 8}, {156, 9}, {157, 10}, {155, 11}, {161, 12}, {159, 13}, {213, 14}, {211, 15}, {216, 16}, {215, 17}};

        Level::Level townLevelBase(Level::Dun::getTown(sector1, sector2, sector3, sector4),
                                   "levels/towndata/town.til",
                                   "levels/towndata/town.min",
                                   "levels/towndata/town.sol",
                                   "levels/towndata/town.cel",
                                   "levels/towndata/towns.cel",
                                   specialCelMap,
                                   Misc::Point(25u, 29u),
                                   Misc::Point(75u, 68u),
                                   std::map<int32_t, int32_t>(),
                                   static_cast<int32_t>(-1),
                                   1);

        auto townLevel = new GameLevel(*this, std::move(townLevelBase), 0);
        mLevels[0] = townLevel;

        for (auto npc : mDiabloExe.getNpcs())
        {
            auto actor = new Actor(*this, *npc, mDiabloExe);
            actor->teleport(townLevel, Position(Misc::Point(npc->x, npc->y), Misc::Direction(static_cast<Misc::Direction8>(npc->rotation))));
        }

        for (int32_t i = 1; i < 17; i++)
        {
            mLevels[i] = nullptr; // let's generate levels on demand
        }
    }

    GameLevel* World::getCurrentLevel() { return mCurrentPlayer->getLevel(); }

    int32_t World::getCurrentLevelIndex() { return mCurrentPlayer->getLevel()->getLevelIndex(); }

    void World::setLevel(int32_t levelNum, bool upStairsPos)
    {
        if (levelNum >= int32_t(mLevels.size()) || levelNum < 0 || (mCurrentPlayer->getLevel() && mCurrentPlayer->getLevel()->getLevelIndex() == levelNum))
            return;

        // Clear atlas texture
        Engine::ThreadManager::get()->clearSprites();

        auto level = getLevel(levelNum);

        auto pos = upStairsPos ? level->upStairsPos() : level->downStairsPos();
        mCurrentPlayer->teleport(level, FAWorld::Position(pos));
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
            case 1:
            case 2:
            case 3:
            case 4:
            {
                threadManager->playMusic("music/dlvla.wav");
                break;
            }
            case 5:
            case 6:
            case 7:
            case 8:
            {
                threadManager->playMusic("music/dlvlb.wav");
                break;
            }
            case 9:
            case 10:
            case 11:
            case 12:
            {
                threadManager->playMusic("music/dlvlc.wav");
                break;
            }
            case 13:
            case 14:
            case 15:
            case 16:
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
        auto p = mLevels.find(level);
        if (p == mLevels.end())
            return nullptr;
        if (p->second == nullptr)
        {
            p->second = FALevelGen::generate(*this, *mLevelRng.get(), 100, 100, level, mDiabloExe, level - 1, level + 1);
        }
        return p->second;
    }

    void World::insertLevel(size_t level, GameLevel* gameLevel) { mLevels[level] = gameLevel; }

    Actor* World::getActorAt(const Misc::Point& point) { return getCurrentLevel()->getActorAt(point); }

    void World::update(bool noclip, const std::vector<PlayerInput>& inputs)
    {
        mTicksPassed++;

        for (const auto& input : inputs)
        {
            switch (input.mType)
            {
                case PlayerInput::Type::PlayerJoined:
                {
                    if (Engine::EngineMain::get()->mMultiplayer->isPlayerRegistered(input.mData.dataPlayerJoined.peerId))
                        break;

                    PlayerClass playerClass = mNextPlayerClass;

                    // hacky method to make different players use different classes
                    // TODO: remove this when we have a proper character system
                    mNextPlayerClass = PlayerClass((int32_t(mNextPlayerClass) + 1) % 3);

                    FAWorld::Player* newPlayer = Engine::EngineMain::get()->mPlayerFactory->create(*this, playerClass);
                    registerPlayer(newPlayer);
                    FAWorld::GameLevel* level = getLevel(0);

                    newPlayer->teleport(level, FAWorld::Position(level->getFreeSpotNear(level->upStairsPos())));
                    Engine::EngineMain::get()->mMultiplayer->registerNewPlayer(newPlayer, input.mData.dataPlayerJoined.peerId);

                    break;
                }
                case PlayerInput::Type::PlayerLeft:
                {
                    // a little unsubtle, but it'll do for now.
                    if (Actor* actor = getActorById(input.mActorId))
                        actor->die();
                    break;
                }
                default:
                {
                    if (Player* player = dynamic_cast<Player*>(this->getActorById(input.mActorId)))
                        player->getPlayerBehaviour()->addInput(input);
                    break;
                }
            }
        }

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

    Player* World::getCurrentPlayer() { return mCurrentPlayer; }

    void World::addCurrentPlayer(Player* player)
    {
        debug_assert(mCurrentPlayer == nullptr || mCurrentPlayer == player);
        mCurrentPlayer = player;
    }

    void World::registerPlayer(Player* player)
    {
        auto sortedInsertPosIt =
            std::upper_bound(mPlayers.begin(), mPlayers.end(), player, [](Player* lhs, Player* rhs) { return lhs->getId() > rhs->getId(); });

        mPlayers.insert(sortedInsertPosIt, player);
    }

    void World::deregisterPlayer(Player* player) { mPlayers.erase(std::find(mPlayers.begin(), mPlayers.end(), player)); }

    const std::vector<Player*>& World::getPlayers() { return mPlayers; }

    void World::fillRenderState(FARender::RenderState* state, const HoverStatus& hoverStatus)
    {
        if (getCurrentLevel())
            getCurrentLevel()->fillRenderState(state, getCurrentPlayer(), hoverStatus);
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

    Tick World::getCurrentTick() { return mTicksPassed; }

    PlacedItemData* World::targetedItem(Misc::Point screenPosition)
    {
        auto tile = getTileByScreenPos(screenPosition);
        return getCurrentLevel()->getItemMap().getItemAt(tile.pos);
    }

    Tick World::getTicksInPeriod(FixedPoint seconds) { return std::max(Tick(1), Tick((FixedPoint(ticksPerSecond) * seconds).round())); }

    FixedPoint World::getSecondsPerTick() { return FixedPoint(1) / FixedPoint(ticksPerSecond); }
}
