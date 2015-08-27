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

        private:
            void runGameLoop(const boost::program_options::variables_map& variables);

            EngineInputManager* mInputManager = NULL;
    };
}

#endif // ENGINEMAIN_H
