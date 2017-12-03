#ifndef ENGINEMAIN_H
#define ENGINEMAIN_H
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
    class EngineMain : public KeyboardInputObserverInterface
    {
    public:
        EngineInputManager& inputManager();
        void run(const boost::program_options::variables_map& variables);
        void stop();
        void togglePause();
        void toggleNoclip();
        void notify(KeyboardInputAction action);
        void startGame();

    private:
        void runGameLoop(const boost::program_options::variables_map& variables, const std::string& pathEXE);

    private:
        std::unique_ptr<FAWorld::World> mWorld;
        std::shared_ptr<EngineInputManager> mInputManager;
        bool mDone = false;
        bool mPaused = false;
        bool mNoclip = false;
        bool inGame = false;
    };
}

#endif // ENGINEMAIN_H
