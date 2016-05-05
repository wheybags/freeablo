#ifndef ENGINEMAIN_H
#define ENGINEMAIN_H

#include <boost/program_options.hpp>
#include "input.h"

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
            ~EngineMain();
            static EngineInputManager* inputManager();
            void run(const boost::program_options::variables_map& variables);
            void stop();
            void pause();
            void unPause();
            void toggleNoclip();
            void notify(KeyboardInputAction action);

        private:
            void runGameLoop(const boost::program_options::variables_map& variables, const std::string& pathEXE);

            static EngineInputManager* mInputManager;
            bool mDone = false;
            bool mPaused = false;
            bool mNoclip = false;
    };
}

#endif // ENGINEMAIN_H
