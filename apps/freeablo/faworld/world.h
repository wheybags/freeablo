
#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "../engine/inputobserverinterface.h"
#include "../fasavegame/objectidmapper.h"

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
    class DialogManager;
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
    class HoverState;
    class PlacedItemData;

    // at 125 ticks/second, it will take about 2 billion years to reach max (signed) value, so int64 will probably do :p
    typedef int64_t Tick;
    static const Tick MAX_TICK = 9223372036854775807;

    class World : public Engine::KeyboardInputObserverInterface
    {
    public:
        World(const DiabloExe::DiabloExe& exe);
        World(FASaveGame::GameLoader& loader, const DiabloExe::DiabloExe& exe);
        void save(FASaveGame::GameSaver& saver);
        ~World();

        static World* get();
        void notify(Engine::KeyboardInputAction action);
        Render::Tile getTileByScreenPos(Misc::Point screenPos);
        Actor* targetedActor(Misc::Point screenPosition);
        PlacedItemData* targetedItem(Misc::Point screenPosition);
        void updateHover(const Misc::Point& mousePosition);
        void generateLevels();
        GameLevel* getCurrentLevel();
        int32_t getCurrentLevelIndex();

        void setLevel(int32_t levelNum);
        GameLevel* getLevel(size_t level);
        void insertLevel(size_t level, GameLevel* gameLevel);

        Actor* getActorAt(size_t x, size_t y);

        void update(bool noclip);

        void addCurrentPlayer(Player* player);
        Player* getCurrentPlayer();

        void registerPlayer(Player* player);
        void deregisterPlayer(Player* player);
        const std::vector<Player*>& getPlayers();

        void fillRenderState(FARender::RenderState* state);

        static Tick getTicksInPeriod(float seconds);
        static float getSecondsPerTick();

        Actor* getActorById(int32_t id);

        void getAllActors(std::vector<Actor*>& actors);

        Tick getCurrentTick();
        void setGuiManager(FAGui::GuiManager* manager);
        HoverState& getHoverState();

        void setupObjectIdMappers();

        int32_t getNewId() { return mNextId++; }

        void blockInput();
        void unblockInput();

        static const Tick ticksPerSecond = 125; ///< number of times per second that game state will be updated
        FASaveGame::ObjectIdMapper mObjectIdMapper;
        FAGui::GuiManager* mGuiManager = nullptr;
        std::unique_ptr<FAGui::DialogManager> mDlgManager;

    private:
        void playLevelMusic(size_t level);
        void changeLevel(bool up);

        std::map<int32_t, GameLevel*> mLevels;
        Tick mTicksPassed = 0;
        Player* mCurrentPlayer = nullptr;
        std::vector<Player*> mPlayers; ///< This vector is sorted
        const DiabloExe::DiabloExe& mDiabloExe;

        int32_t mNextId = 1;
    };
}
