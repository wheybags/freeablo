#ifndef W_MONSTER_H
#define W_MONSTER_H

#include "actor.h"

namespace DiabloExe
{
    struct Monster;
}

namespace FAWorld
{
    class Monster: public Actor
    {
        public:
            Monster(const DiabloExe::Monster& monster, Position pos, ActorStats * stats=nullptr);
            std::string getDieWav();
            std::string getHitWav();


        private:
            std::string getWalkCl2(const DiabloExe::Monster& monster);
            std::string getIdleCl2(const DiabloExe::Monster& monster);
            std::string getDieCl2(const DiabloExe::Monster & monster);

    };
}

#endif     
