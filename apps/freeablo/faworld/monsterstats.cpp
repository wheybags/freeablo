#include "monsterstats.h"
#include "../falevelgen/random.h"
#include "monster.h"

namespace FAWorld
{
    MonsterStats::MonsterStats(const DiabloExe::Monster &monsterStat):
        mMinHP(monsterStat.minHp),
        mMaxHP(monsterStat.maxHp),
        mArmourClass(monsterStat.armourClass),
        mLevel(monsterStat.level)

    {        
        mHP = FALevelGen::randomInRange(mMinHP, mMaxHP);
        mCurrentHP=mHP;
    }

    void MonsterStats::takeDamage(double amount)
    {
        mCurrentHP -= amount;
    }
}
