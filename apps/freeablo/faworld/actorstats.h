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
            typedef enum
            {
                Strength,
                Magic,
                Dexterity,
                Vitality
            } BasicStat;

            /*{
                std::cout << __LINE__ << " here" << std::endl;
                return 0;
            }*/






            ActorStats(){};

            ActorStats(const DiabloExe::Monster & monsterStat);

            int32_t getCurrentHP()
            {
                return 0;
            }
            double getAttackSpeed()
            {
                return 0.0;
            }

            uint32_t getArmourClass()
            {
                return 0;
            }

            uint32_t getLevel()
            {
                return 0;
            }


            virtual void recalculateDerivedStats(){}

        protected:            
            virtual void clearDerivedStats();
            uint32_t mFirstAttackDamageMin = 0;
            uint32_t mFirstAttackDamageMax = 0;
            uint32_t mSecondAttackDamageMin= 0;
            uint32_t mSecondAttackDamageMax= 0;

            uint32_t mMonsterType;
            uint8_t mLevelPointsToSpend;

    };
}



#endif //ACTORSTATS_H
