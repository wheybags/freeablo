#include "actorstats.h"
#include "../falevelgen/random.h"
#include "item.h"
namespace FAWorld
{

    ActorStats::ActorStats(const DiabloExe::Monster &monsterStat)
        : mMaxVitality(0), mMaxMagic(0),
          mMaxDexterity(0), mMaxStrength(0)
    {
        mHP = FALevelGen::randomInRange(monsterStat.minHp, monsterStat.maxHp);
        mCurrentHP = mHP;
        mDamageDoneMelee = FALevelGen::randomInRange(monsterStat.minDamage, monsterStat.maxDamage);
        mSecondAttackDamageDone = FALevelGen::randomInRange(monsterStat.maxDamageSecond, monsterStat.maxDamageSecond);
        mMonsterType = monsterStat.type;
        mArmourClass = monsterStat.armourClass;
    }

    bool ActorStats::levelUp(BasicStat statModified)
    {
        UNUSED_PARAM(statModified);
        mLevelPointsToSpend=5;
        mLevelPoints--;
        mLevel++;
        mExpToNextLevel = mExpForLevel[mLevel-1];
        recalculateDerivedStats();
        return true;
    }

    bool ActorStats::spendLevelUpPoint(BasicStat stat)
    {
        if(mLevelPointsToSpend>0)
        {
            switch(stat)
            {
                case Strength:
                    mSpentLevelsOnStrength++;
                    break;
                case Magic:
                    mSpentLevelsOnMagic++;
                    break;
                case Dexterity:
                    mSpentLevelsOnDexterity++;
                    break;
                case Vitality:
                    mSpentLevelsOnVitality++;
                    break;
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    void ActorStats::printStats()
    {
        printf("Level: %d\n", mLevel);
        printf("ExpToNextLevel: %d\n", mExpToNextLevel);
        printf("MeleeDamage: %f\n", mDamageDoneMelee);
        printf("BowDamage: %f\n", mDamageDoneBow);
        printf("Strength: %d\n", mStrength);
        printf("Magic: %d\n", mMagic);
        printf("Dexterity: %d\n", mDexterity);
        printf("ChanceToHitScreen: %f\n", mChanceToHitCharacterScreen);
        printf("ChanceToHitArrowBase: %f\n", mChanceToHitArrow);
        printf("ChanceToHitMeleeBase: %f\n", mChanceToHitMelee);
        printf("Vitality: %d\n", mVitality);
        printf("HP: %d\n", mHP);
        printf("Mana: %d\n", mMana);
        printf("ArmourClass: %f\n", mArmourClass);

    }

    void ActorStats::clearDerivedStats()
    {
        mHP=0;
        //mCurrentHP=0;
        mMana=0;
        mCurrentMana=0;
        mStrength=0;
        mVitality=0;
        mDexterity=0;
        mMagic=0;
        mResistanceFire=0;
        mResistanceLightning=0;
        mResistanceMagic=0;
        mResistanceAll=0;
        mChanceToHitCharacterScreen=0;
        mChanceToHitMelee=0;
        mChanceToHitArrow=0;
        mChanceToHitMagic=0;
        mChanceToHitMagicVPlayer=0;
        mArmourClass=0;
        mManaShield=0;
        mDamageDone=0;
        //mAttackSpeed=0;
        mDamageDoneBow=0;
        mDamageDoneMelee=0;
        mBonusStrength=0;
        mBonusMagic=0;
        mBonusDexterity=0;
        mBonusVitality=0;
        mHasThieves=false;
    }

    int32_t ActorStats::getCurrentHP()
    {
        return mCurrentHP;
    }

    void ActorStats::takeDamage(int32_t amount)
    {
        mCurrentHP -= amount;
    }

    double ActorStats::getMeleeDamage()
    {
        return mDamageDoneMelee;
    }

    uint8_t ActorStats::getAttackSpeed()
    {
        return mAttackSpeed;
    } 

    uint8_t ActorStats::getAttackFrameset()
    {
        return mAttackFrameset;
    }







}
