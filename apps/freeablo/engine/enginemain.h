#ifndef ENGINEMAIN_H
#define ENGINEMAIN_H
#include <memory>
#include <boost/program_options.hpp>
#include "engineinputmanager.h"

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

namespace FAGui
{
    class GuiManager;
}

namespace Engine
{
    class EngineMain : public KeyboardInputObserverInterface
    {
        public:
            EngineInputManager& inputManager();
            void run(const boost::program_options::variables_map& variables);
            void stop();
            void pause();
            void unPause();
            void toggleNoclip();
            void notify(KeyboardInputAction action);

        private:
            void runGameLoop(const boost::program_options::variables_map& variables, const std::string& pathEXE);

            std::shared_ptr<EngineInputManager> mInputManager;
            std::shared_ptr<FAGui::GuiManager> mGuiManager;
            bool mDone = false;
            bool mPaused = false;
            bool mNoclip = false;
    };
}

#endif // ENGINEMAIN_H
