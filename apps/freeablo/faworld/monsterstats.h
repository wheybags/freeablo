#ifndef MONSTERSTATS_H
#define MONSTERSTATS_H
#include "actorstats.h"
#include <diabloexe/diabloexe.h>
namespace FAWorld
{
    class MonsterStats : public ActorStats
    {
        public:

            MonsterStats(const DiabloExe::Monster &monsterStat){}


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
        private:
            uint32_t mFirstAttackDamageMin = 0;
            uint32_t mFirstAttackDamageMax = 0;
            uint32_t mSecondAttackDamageMin= 0;
            uint32_t mSecondAttackDamageMax= 0;
            uint32_t mMonsterType=0;

    };


}


#endif // MONSTERSTATS_H
