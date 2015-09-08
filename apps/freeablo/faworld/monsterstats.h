#ifndef MONSTERSTATS_H
#define MONSTERSTATS_H
#include "actorstats.h"
#include <diabloexe/diabloexe.h>

namespace FAWorld
{
    class Monster;
    class MonsterStats : public ActorStats
    {
        public:

            MonsterStats(const DiabloExe::Monster &monsterStat);
            Monster * mMonster;


            int32_t getCurrentHP()
            {
                return mCurrentHP;
            }

            double getAttackSpeed()
            {
                return 0.0;
            }

            uint32_t getArmourClass()
            {
                return mArmourClass;
            }

            uint32_t getLevel()
            {
                return 0;
            }

            void takeDamage(double amount);
            void setActor(Monster * actor)
            {
                mMonster = actor;
            }

        private:
            int32_t mCurrentHP=0;
            uint32_t mHP=0;
            uint32_t mMinHP=0;
            uint32_t mMaxHP=0;
            uint32_t mFirstAttackDamageMin = 0;
            uint32_t mFirstAttackDamageMax = 0;
            uint32_t mSecondAttackDamageMin= 0;
            uint32_t mSecondAttackDamageMax= 0;
            uint32_t mMonsterType=0;
            uint32_t mArmourClass=0;



    };


}


#endif // MONSTERSTATS_H
