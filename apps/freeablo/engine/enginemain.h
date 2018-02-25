
#pragma once

#include "../faworld/playerfactory.h"
#include "engineinputmanager.h"
#include <boost/program_options.hpp>
#include <memory>

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

namespace Engine
{
    class LocalInputHandler;

    class EngineMain : public KeyboardInputObserverInterface
    {
    public:
        EngineMain();
        ~EngineMain();
        EngineInputManager& inputManager();
        void run(const boost::program_options::variables_map& variables);
        void stop();
        void togglePause();
        void toggleNoclip();
        void notify(KeyboardInputAction action);
        void setupNewPlayer(FAWorld::Player* player);
        // TODO: replace with enums
        void startGame(const std::string& characterClass);
        const DiabloExe::DiabloExe& exe() const;
        bool isPaused() const;

        static EngineMain* get() { return singletonInstance; }
        LocalInputHandler* getLocalInputHandler() { return mLocalInputHandler.get(); }

    private:
        void runGameLoop(const boost::program_options::variables_map& variables, const std::string& pathEXE);

    private:
        static EngineMain* singletonInstance;

        std::unique_ptr<LocalInputHandler> mLocalInputHandler;
        std::unique_ptr<FAWorld::World> mWorld;
        std::shared_ptr<EngineInputManager> mInputManager;
        std::unique_ptr<DiabloExe::DiabloExe> mExe;
        std::unique_ptr<FAWorld::PlayerFactory> mPlayerFactory;
        std::unique_ptr<FAGui::GuiManager> mGuiManager;
        FAWorld::Player* mPlayer = nullptr;
        bool mDone = false;
        bool mPaused = false;
        bool mNoclip = false;
        bool mInGame = false;
    };
}
