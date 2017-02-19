#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <map>
#include <utility>

#include "../engine/inputobserverinterface.h"

#include <boost/optional/optional.hpp> // TODO: replace with std::optional when available
#include <boost/signals2/signal.hpp>
#include "../fagui/guimanager.h"

namespace Render
{
  struct Tile;
}

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
    class Actor;
    class Player;
    class GameLevel;

    enum class HoverType
    {
      actor,
      none,
    };

    struct HoverState // TODO: move to some other place, maybe even guimanager
    {
      HoverType type = HoverType::none;
      int32_t actorId;

    public:
      HoverState () {}
      bool applyIfNeeded(const HoverState& newState);
      bool operator==(const HoverState& other) const;
      // for now this function if state was applied and if it was caller should ask guimanager to update status bar
      // later on logic probably will be different.
      bool actorHovered(int32_t actorIdArg);
      bool nothingHovered();

    private:
      HoverState (HoverType typeArg) : type (typeArg) {}
    };

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
            static float getSecondsPerTick();

            Actor* getActorById(int32_t id);

            void getAllActors(std::vector<Actor*>& actors);
            void setGuiManager(FAGui::GuiManager* guiManager);

        private:
            void playLevelMusic(size_t level);
            void changeLevel(bool up);
            void stopPlayerActions();
            void onMouseClick(Engine::Point mousePosition);
            Render::Tile getTileByScreenPos(Engine::Point screenPos);
            void onMouseMove(Engine::Point mousePosition);
            void onMouseDown(Engine::Point mousePosition);

            std::map<size_t, GameLevel*> mLevels;
            HoverState m_hoverState;
            size_t mTicksPassed = 0;
            Player* mCurrentPlayer;
            std::vector<Player*> mPlayers;
            const DiabloExe::DiabloExe& mDiabloExe;
            std::pair<int32_t, int32_t> mDestination;       ///< this is the aim point to move to.
            FAGui::GuiManager* mGuiManager;
    };
}

#endif
