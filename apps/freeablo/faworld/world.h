#pragma once
#include "../engine/inputobserverinterface.h"
#include "../fasavegame/objectidmapper.h"
#include "enums.h"
#include "playerinput.h"
#include <map>
#include <memory>
#include <misc/fixedpoint.h>
#include <utility>
#include <vector>

namespace Random
{
    class Rng;
}

namespace FARender
{
    class RenderState;
}

namespace DiabloExe
{
    class DiabloExe;
}

namespace FAGui
{
    class GuiManager;
}

namespace Render
{
    struct Tile;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class Actor;
    class Player;
    class GameLevel;
    class PlacedItemData;
    class ItemFactory;
    class HoverStatus;
    class StoreData;

    // at 125 ticks/second, it will take about 2 billion years to reach max (signed) value, so int64 will probably do :p
    typedef int64_t Tick;
    static const Tick MAX_TICK = 9223372036854775807;

    class World
    {
    public:
        World(const DiabloExe::DiabloExe& exe, uint32_t seed);
        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);
        ~World();

        void setFirstPlayerAsCurrent();

        Render::Tile getTileByScreenPos(Misc::Point screenPos);
        Actor* targetedActor(Misc::Point screenPosition);
        PlacedItemData* targetedItem(Misc::Point screenPosition);
        void updateHover(const Misc::Point& mousePosition);
        void generateLevels();
        GameLevel* getCurrentLevel();
        int32_t getCurrentLevelIndex();

        void setLevel(int32_t levelNum, bool upStairsPos = true);
        GameLevel* getLevel(size_t level);
        void insertLevel(size_t level, GameLevel* gameLevel);
        void regenerateStoreItems();

        Actor* getActorAt(const Misc::Point& point);

        void update(bool noclip, const std::vector<PlayerInput>& inputs);

        void addCurrentPlayer(Player* player);
        Player* getCurrentPlayer();

        void registerPlayer(Player* player);
        void deregisterPlayer(Player* player);
        const std::vector<Player*>& getPlayers();

        void fillRenderState(FARender::RenderState* state, const HoverStatus& hoverStatus);

        static Tick getTicksInPeriod(FixedPoint seconds);
        static FixedPoint getSecondsPerTick();

        Actor* getActorById(int32_t id);

        Tick getCurrentTick();

        void setupObjectIdMappers();

        int32_t getNewId() { return mNextId++; }

        void blockInput();
        void unblockInput();
        const ItemFactory& getItemFactory() const;
        StoreData& getStoreData() { return *mStoreData; }

        void playLevelMusic(size_t level);

        static const Tick ticksPerSecond = 60; ///< number of times per second that game state will be updated
        FASaveGame::ObjectIdMapper mObjectIdMapper;

        const DiabloExe::DiabloExe& mDiabloExe; // TODO: something better than this
        std::unique_ptr<Random::Rng> mRng;

    private:
        std::unique_ptr<Random::Rng> mLevelRng;
        std::map<int32_t, GameLevel*> mLevels;
        Tick mTicksPassed = 0;
        Player* mCurrentPlayer = nullptr;
        std::vector<Player*> mPlayers; ///< This vector is sorted
        std::unique_ptr<ItemFactory> mItemFactory;
        std::unique_ptr<StoreData> mStoreData;

        int32_t mNextId = 1;
        PlayerClass mNextPlayerClass = PlayerClass::warrior;
    };
}
