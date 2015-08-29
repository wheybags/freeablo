#ifndef ENGINEMAIN_H
#define ENGINEMAIN_H

#include <boost/program_options.hpp>

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
    class EngineInputManager;

    class EngineMain
    {
        public:
            ~EngineMain();

            void run(const boost::program_options::variables_map& variables);

            void stop();

        private:
            void runGameLoop(const boost::program_options::variables_map& variables);

            EngineInputManager* mInputManager = NULL;
            bool mDone = false;
    };
}

#endif // ENGINEMAIN_H
