#ifndef ACTORSTATS_H
#define ACTORSTATS_H
#include <stdint.h>
#include <vector>
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

            uint32_t getStrength() const{return mStrenght;}
            uint32_t getMaxStrength() const{return mMaxStrength;}



            void setActor(Actor * actor){mActor = actor;}
            //ActorStats(){}
            ActorStats(uint32_t strength,
                       uint32_t maxStrength,
                       uint32_t magic,
                       uint32_t maxMagic,
                       uint32_t dexterity,
                       uint32_t maxDexterity,
                       uint32_t vitality,
                       uint32_t maxVitality,
                       uint32_t blocking) :
                mLevel(1),
                mLevelPoints(0),
                mExp(0),
                mExpToNextLevel(2000),
                mVitality(vitality),
                mMaxVitality(maxVitality),
                mMagic(magic),
                mMaxMagic(maxMagic),
                mDexterity(dexterity),
                mMaxDexterity(maxDexterity),
                mStrenght(strength),
                mMaxStrength(maxStrength),
                mBlockingBonus(blocking)

            {
                recalculateDerivedStats();
            }

            bool levelUp(BasicStat statModified)
            {
                if(mLevelPoints >= 1)
                {
                    switch(statModified)
                    {
                        case Strength:
                            mStrenght+=1;
                            break;

                        case Magic:
                            mMagic+=1;
                            break;

                        case Dexterity:
                            mDexterity+=1;
                            break;

                        case Vitality:
                            mVitality+=1;
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
            uint32_t mLevel;
            uint32_t mLevelPoints;
            uint32_t mExp;
            uint32_t mExpToNextLevel;

            uint32_t mVitality;
            const uint32_t mMaxVitality;

            uint32_t mMagic;
            const uint32_t mMaxMagic;

            uint32_t mDexterity;
            const uint32_t mMaxDexterity;

            uint32_t mStrenght;
            const uint32_t mMaxStrength;

            uint32_t mBlockingBonus;

            uint32_t mHP;

            uint32_t mMana;

            uint32_t mResistanceFire;
            uint32_t mResistanceLightning;
            uint32_t mResistanceMagic;
            uint32_t mResistanceAll;
            uint32_t mChanceToHit;
            uint32_t mArmourClass;
            uint32_t mManaShield;
            uint32_t mDamageDone;
            uint32_t mAttackSpeed;
            double mWalkSpeed;
            Actor * mActor;
    };
}



#endif //ACTORSTATS_H
