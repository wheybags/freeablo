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
            virtual ~ActorStats() {}

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
                mBlockingBonus(stats.mBlockingBonus),
                mAttackFrameset(stats.mAttackFrameset),
                mAttackSpeed(stats.mSingleHandedAttackSpeed)

            {
                mVitality = mStartingVitality;
                mStrength = mStartingStrength;                
                mMagic    = mStartingMagic;
                mDexterity = mStartingDexterity;
                recalculateDerivedStats();
            }
            ActorStats(const DiabloExe::Monster & monsterStat);

            virtual void printStats();
            bool levelUp(BasicStat statModified);
            bool spendLevelUpPoint(BasicStat stat);
            virtual void takeDamage(int32_t amount);
            virtual double getMeleeDamage();
            virtual uint8_t getAttackSpeed();
            virtual uint8_t getAttackFrameset();
            virtual int32_t getCurrentHP();
            virtual void recalculateDerivedStats(){}

            template <class Stream>
            Serial::Error::Error faSerial(Stream& stream)
            {
                serialise_int32(stream, mStrength);
                serialise_int32(stream, mMagic);
                serialise_int32(stream, mDexterity);
                serialise_int32(stream, mVitality);
                serialise_int32(stream, mMana);
                serialise_int32(stream, mCurrentMana);
                serialise_int32(stream, mHP);
                serialise_int32(stream, mCurrentHP);
                serialise_int32(stream, mMonsterType);

                return Serial::Error::Success;
            }

        protected:
            virtual void clearDerivedStats();

            // these attributes are synced by multiplayer
            // the rest are left at default values on clients for now
            uint32_t mStrength;
            uint32_t mMagic;
            uint32_t mDexterity;
            uint32_t mVitality;
            uint32_t mMana = 0;
            uint32_t mCurrentMana = 0;
            uint32_t mHP = 0;
            int32_t mCurrentHP = 0;
            uint32_t mMonsterType;





            const std::vector<uint32_t> mExpForLevel;
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
            uint8_t mAttackSpeed;
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
            double mSecondAttackDamageDone=0.0;
            uint8_t mLevelPointsToSpend;
            Actor * mActor;
    };
}



#endif //ACTORSTATS_H
