#ifndef ACTORSTATS_H
#define ACTORSTATS_H

#include <stdint.h>
#include <vector>
#include <diabloexe/diabloexe.h>
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

            uint32_t getLevel() const{return mLevel;}
            uint32_t getLevelPoints() const{return mLevelPoints;}
            uint32_t getExp() const{return mExp;}
            uint32_t getExpToNextLevel() const{return mExpToNextLevel;}

            uint32_t getVitality() const{return mVitality;}
            uint32_t getMaxVitality() const{return mMaxVitality;}

            uint32_t getMagic() const{return mMagic;}
            uint32_t getMaxMagic() const{return mMaxMagic;}

            uint32_t getDexterity() const{return mDexterity;}
            uint32_t getMaxDexterity() const{return mMaxDexterity;}

            uint32_t getStrength() const{return mStrength;}
            uint32_t getMaxStrength() const{return mMaxStrength;}

            void setActor(Actor * actor){mActor = actor;}

            ActorStats(DiabloExe::CharacterStats stats) :
                mExpForLevel(stats.mNextLevelExp),
                mLevel(1),
                mLevelPoints(0),
                mExp(0),
                mExpToNextLevel(2000),
                mStartingVitality(stats.mVitality),
                mMaxVitality(stats.mMaxVitality),
                mStartingMagic(stats.mMagic),
                mMaxMagic(stats.mMaxMagic),
                mStartingDexterity(stats.mDexterity),
                mMaxDexterity(stats.mMaxDexterity),
                mStartingStrength(stats.mStrength),
                mMaxStrength(stats.mMaxStrength),
                mBlockingBonus(stats.mBlockingBonus)

            {
                mVitality = mStartingVitality;
                mStrength = mStartingStrength;
                mMagic    = mMagic;
                mDexterity = mStartingDexterity;
                recalculateDerivedStats();
            }
            virtual void processEffects();
            virtual void printStats();
            bool levelUp(BasicStat statModified);
            bool spendLevelUpPoint(BasicStat stat);
            virtual void recalculateDerivedStats(){}

        protected:
            const std::vector<uint32_t> mExpForLevel;
            virtual void clearDerivedStats();
            uint32_t mStrength;
            uint32_t mMagic;
            uint32_t mDexterity;
            uint32_t mVitality;
            uint32_t mLevel;
            uint32_t mLevelPoints;
            uint32_t mExp;
            uint32_t mExpToNextLevel;
            uint32_t mStartingVitality;
            const uint32_t mMaxVitality;
            uint32_t mStartingMagic;
            const uint32_t mMaxMagic;
            uint32_t mStartingDexterity;
            const uint32_t mMaxDexterity;
            uint32_t mStartingStrength;
            const uint32_t mMaxStrength;
            uint32_t mBlockingBonus;
            uint8_t mBlockingSpeed;
            uint8_t mSpellCastingSpeed;
            uint8_t mWalkInTownFrameset;
            uint8_t mWalkInDungeonFrameset;
            uint8_t mDeathFrameset;
            uint8_t mMagicCastFrameset;
            uint8_t mAttackFrameset;
            uint8_t mIdleInTownFrameset;
            uint8_t mIdleInDungeonFrameset;
            uint32_t mSpentLevelsOnStrength=0;
            uint32_t mSpentLevelsOnMagic=0;
            uint32_t mSpentLevelsOnDexterity=0;
            uint32_t mSpentLevelsOnVitality=0;
            uint32_t mMana=0;
            uint32_t mCurrentMana=0;
            uint32_t mHP=0;
            uint32_t mCurrentHP=0;
            uint32_t mBonusStrength=0;
            uint32_t mBonusMagic=0;
            uint32_t mBonusDexterity=0;
            uint32_t mBonusVitality=0;
            double mResistanceFire=0;
            double mResistanceLightning=0;
            double mResistanceMagic=0;
            double mResistanceAll=0;
            double mChanceToHitCharacterScreen=0;
            double mChanceToHitMelee=0;
            double mChanceToHitArrow=0;
            double mChanceToHitMagic=0;
            double mChanceToHitMagicVPlayer=0;
            double mBlockingChance=0;
            double mBlockingChanceTrap=0;
            double mBlockingChancePVP=0;
            double mArmourClass=0;
            double mManaShield=0;
            double mDamageDone=0;
            double mAttackSpeed=0;
            double mDamageDoneBow=0;
            double mDamageDoneMelee=0;
            int32_t  mDamageTakenMultiplier=0;
            uint32_t mLightRadius = 0;
            bool mHasThieves=false;
            bool mBonusAgainstDemons=false;
            bool mBonusAgainstUndead=false;
            double mWalkSpeed=0.4;
            double mSwingSpeed=0.0;
            double mHitRecovery=0.0;
            uint8_t mLevelPointsToSpend;
            Actor * mActor;
    };
}



#endif //ACTORSTATS_H
