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
    class ThreadManager;

    class EngineMain
    {
        public:
        //private:
            void setLevel(size_t dLvl, const DiabloExe::DiabloExe& exe, FAWorld::World& world, Level::Level* level);
            Level::Level* getLevel(size_t dLvl, const DiabloExe::DiabloExe& exe);
            void playLevelMusic(int32_t currentLevel, Engine::ThreadManager& threadManager);

            void run(const boost::program_options::variables_map& variables);
            void runGameLoop(const boost::program_options::variables_map& variables);
    };
}

#endif // ENGINEMAIN_H
