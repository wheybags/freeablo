#ifndef STATS_H
#define STATS_H

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
            uint32_t getMaxMagic() const{returm mMaxMagic;}

            uint32_t getDexterity() const{return mDexterity;}
            uint32_t getMaxDexterity() const{return mMaxDexterity;}

            uint32_t getStrength() const{return mStrenght;}
            uint32_t getMaxStrength() const{return mMaxStrength;}
            ActorStats(){}
            ActorStats(Actor * actor,
                       uint32_t strength,
                       uint32_t maxStrength,
                       uint32_t magic,
                       uint32_t maxMagic,
                       uint32_t dexterity,
                       uint32_t maxDexterity,
                       uint32_t vitality,
                       uint32_t maxVitality,
                       uint32_t blocking)
            {
                mActor = actor;

                mStrenght = strength;
                mMaxStrength = maxStrength;

                mMagic = magic;
                mMaxMagic = maxMagic;

                mDexterity = dexterity;
                mMaxDexterity = maxDexterity;

                mVitality = vitality;
                mMaxVitality = maxVitality;

                mBlockingBonus = blocking;

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
                    recalculateDerivedStats();
                    return true;
                }
                else
                {
                    return false;
                }
            }

        private:
            virtual void recalculateDerivedStats() = 0;
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

            uint32_t mChanceToHit;
            uint32_t mArmourClass;
            uint32_t mManaShield;
            uint32_t mDamageDone;
            uint32_t mAttackSpeed;
            unsigned double mWalkSpeed;
            Actor * mActor;
    };
}



#endif STATS_H
