#include "monster.h"

#include <diabloexe/monster.h>

#include "../falevelgen/random.h"
#include "../engine/threadmanager.h"
#include <boost/format.hpp>

namespace FAWorld
{
    Monster::Monster(const DiabloExe::Monster& monster, Position pos, ActorStats *stats):
        Actor(getWalkCl2(monster), getIdleCl2(monster), pos, getDieCl2(monster), stats, monster.soundPath)
    {}

    std::string Monster::getWalkCl2(const DiabloExe::Monster& monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'w';

        return fmt.str();
    }
    
    std::string Monster::getIdleCl2(const DiabloExe::Monster& monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'n';

        return fmt.str();
    }


    std::string Monster::getDieCl2(const DiabloExe::Monster & monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'd';

        return fmt.str();

    }

    std::string Monster::getDieWav()
    {
        if(mSoundPath.empty())
            printf("Ayy Lmao®®®®®\n");
        boost::format fmt(mSoundPath);
        fmt % 'd';
        return (fmt % FALevelGen::randomInRange(1, 2)).str();

    }

}
