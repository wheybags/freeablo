#ifndef ACTORSTATS_H
#define ACTORSTATS_H
#include <stdint.h>
#include <vector>
#include <diabloexe/diabloexe.h>
namespace FAWorld
{
    class Actor;
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

            bool levelUp(BasicStat statModified)
            {
                if(mLevelPoints >= 1)
                {
                    switch(statModified)
                    {
                        case Strength:
                            mSpentLevelsOnStrength+=1;
                            break;

                        case Magic:
                            mSpentLevelsOnMagic+=1;
                            break;

                        case Dexterity:
                            mSpentLevelsOnDexterity+=1;
                            break;

                        case Vitality:
                            mSpentLevelsOnVitality+=1;
                            break;

                        default:
                            return false;
                    }
                    mLevelPoints--;
                    mLevel++;
                    mExpToNextLevel = mExpForLevel[mLevel-1];
                    recalculateDerivedStats();
                    return true;
                }
                else
                {
                    return false;
                }
            }

            virtual void recalculateDerivedStats(){}

        protected:
            const uint32_t mExpForLevel[50] = {2000,2620,3420,4449,5769,7454,9597,12313,15742,20055,25460,32207,40597,49392,82581,110411,147123,195379,258585,341073,448341,587327,766756,997549,1293323,1670973,2151378,2760218,3528939,4495869,5707505,7219994,9100803,11430609,14305407,17838843,22164762,27439976,33847210,41598222,50937022,62143167,75535020,91472909,110362065,132655203,158854605,190228390,227798497,272788700};
            virtual void clearDerivedStats()
            {
                mHP=0;
                mCurrentHP=0;

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
                mAttackSpeed=0;

                mDamageDoneBow=0;
                mDamageDoneMelee=0;

                mBonusStrength=0;
                mBonusMagic=0;
                mBonusDexterity=0;
                mBonusVitality=0;

                mHasThieves=false;


            }
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
            Actor * mActor;
    };
}



#endif //ACTORSTATS_H
