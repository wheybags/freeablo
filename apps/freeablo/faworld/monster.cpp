#include "monster.h"

#include <diabloexe/monster.h>

#include <boost/format.hpp>

namespace FAWorld
{
    Monster::Monster(const DiabloExe::Monster& monster, Position pos):
        Actor(getWalkCl2(monster), getIdleCl2(monster), pos)
    {}

    std::string Monster::getWalkCl2(const DiabloExe::Monster& monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'n';

        return fmt.str();
    }
    
    std::string Monster::getIdleCl2(const DiabloExe::Monster& monster)
    {
        boost::format fmt(monster.cl2Path);
        fmt % 'n';

        return fmt.str();
    }
}
