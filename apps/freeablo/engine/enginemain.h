#pragma once
#include "../faworld/playerfactory.h"
#include "engineinputmanager.h"
#include <memory>
#include <settings/settings.h>

namespace Level
{
    class Level;
}

namespace FAWorld
{
    class World;
}

namespace DiabloExe
{
    class DiabloExe;
}

namespace cxxopts
{
    class ParseResult;
}

namespace Engine
{
    class LocalInputHandler;
    class MultiplayerInterface;

    class EngineMain : public KeyboardInputObserverInterface
    {
    public:
        EngineMain();
        ~EngineMain() override;
        EngineInputManager& inputManager();
        void run(const cxxopts::ParseResult& variables);
        void stop();
        void togglePause();
        void toggleNoclip();
        void notify(KeyboardInputAction action) override;
        void setupNewPlayer(FAWorld::Player* player);
        // TODO: replace with enums
        void startGame(FAWorld::PlayerClass characterClass);
        void startGameFromSave(const std::string& savePath);
        void startMultiplayerGame(const std::string& serverAddress);
        const DiabloExe::DiabloExe& exe() const;
        bool isPaused() const;

        static EngineMain* get() { return singletonInstance; }
        LocalInputHandler* getLocalInputHandler() { return mLocalInputHandler.get(); }

    private:
        void runGameLoop(const cxxopts::ParseResult& variables, const std::string& pathEXE);

    private:
        static EngineMain* singletonInstance;

        std::unique_ptr<LocalInputHandler> mLocalInputHandler;

    public: // HACK
        std::unique_ptr<FAWorld::World> mWorld;
        std::unique_ptr<MultiplayerInterface> mMultiplayer;
        std::shared_ptr<EngineInputManager> mInputManager;
        std::unique_ptr<DiabloExe::DiabloExe> mExe;
        std::unique_ptr<FAWorld::PlayerFactory> mPlayerFactory;
        std::unique_ptr<FAGui::GuiManager> mGuiManager;
        bool mDone = false;
        bool mPaused = false;
        bool mNoclip = false;
        bool mInGame = false;
        Settings::Settings mSettings;
    };
}
