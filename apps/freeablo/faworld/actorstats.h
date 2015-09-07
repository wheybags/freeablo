#ifndef ACTORSTATS_H
#define ACTORSTATS_H

#include <stdint.h>
#include <vector>
#include <diabloexe/diabloexe.h>
#include <diabloexe/monster.h>
#include "actor.h"


namespace FAWorld
{

    class ActorStats
    {
        public:

            ActorStats(){}

            ActorStats(const DiabloExe::Monster & monsterStat);
            virtual void recalculateDerivedStats(){}

        protected:            
            virtual void clearDerivedStats();


    };
}



#endif //ACTORSTATS_H
