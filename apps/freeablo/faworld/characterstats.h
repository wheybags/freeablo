#ifndef DIABLOCHARACTERSTATS_H
#define DIABLOCHARACTERSTATS_H
#include "actorstats.h"
#include "monsterstats.h"
#include "item.h"
//Stats Constants
#define CHANCE_TO_HIT_BASE 50
#define WARRIOR_MELEE_HIT_CHANCE_BONUS 20
namespace FAWorld
{
    class Inventory;
    class Player;

    class CharacterStatsBase : public ActorStats
    {
        public:
            CharacterStatsBase(DiabloExe::CharacterStats stats, Player * player) :
                mExpForLevel(stats.mNextLevelExp),
                mLevel(1),
                mLevelPoints(0),
                mExp(0),
                mExpToNextLevel(mExpForLevel[0]),
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
                mAttackSpeed(stats.mSingleHandedAttackSpeed),
                mPlayer(player)
            {
                mVitality = mStartingVitality;
                mStrength = mStartingStrength;
                mMagic    = mStartingMagic;
                mDexterity = mStartingDexterity;
                recalculateDerivedStats();
            }

            virtual void processEffects();
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
            uint32_t mStrength;
            double getArmourClass();
            void takeDamage(double amount);
            virtual double getChanceToHitMelee(MonsterStats * enemy);
            virtual double getMeleeDamage(MonsterStats * enemy){UNUSED_PARAM(enemy); return 0;}
            uint8_t getAttackSpeed(){return mAttackSpeed;}
            int32_t getCurrentHP(){return mCurrentHP;}
            virtual void recalculateDerivedStats(){}
            virtual void clearDerivedStats(){}



        protected:
            double getItemDamage();
            double getMonsterBonusDamage(MonsterStats * enemy);
            Item getCurrentWeapon();
            uint32_t mMagic;
            uint32_t mDexterity;
            uint32_t mVitality;
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
            uint32_t mMana=0;
            uint32_t mCurrentMana=0;
            uint32_t mHP=0;
            int32_t mCurrentHP=0;
            uint32_t mBonusStrength=0;
            uint32_t mBonusMagic=0;
            uint32_t mBonusDexterity=0;
            uint32_t mBonusVitality=0;
            double mArmourClass=0;
            double mManaShield=0;            
            uint8_t mAttackSpeed;
            int32_t  mDamageTakenMultiplier=0;
            uint32_t mLightRadius = 0;
            bool mHasThieves=false;
            bool mBonusAgainstDemons=false;
            bool mBonusAgainstUndead=false;
            bool mBonusAgainstAnimal=false;
            Player * mPlayer;

    };

    class MeleeStats : public CharacterStatsBase
    {
    public:
        MeleeStats(DiabloExe::CharacterStats stats, Player * player) : CharacterStatsBase(stats, player)
        {
            mHP = 2*mVitality + 2*mLevel+18;
            mCurrentHP = mHP;
            mMana = mMagic + mLevel -1;


        }
        virtual double getMeleeDamage(MonsterStats *enemy) final;
        void recalculateDerivedStats() final;
    private:

        friend class Inventory;

    };

    class RangerStats : public CharacterStatsBase
    {
        public:
            RangerStats(DiabloExe::CharacterStats stats, Player * player) : CharacterStatsBase(stats, player)
            {
                mHP = mVitality + 2*mLevel+23;
                mCurrentHP = mHP;
                mMana = mMagic + 2*mLevel +5;
            }
            virtual double getMeleeDamage(MonsterStats *enemy) final;
            void recalculateDerivedStats() final;
        private:

            friend class Inventory;

    };

    class MageStats : public CharacterStatsBase
    {
        public:

            MageStats(DiabloExe::CharacterStats stats, Player * player) : CharacterStatsBase(stats, player)
            {
                mHP = mVitality + 2*mLevel + 9;
                mCurrentHP = mHP;
                mMana = 2*mMagic + 2*mLevel -2;
            }
            virtual double getMeleeDamage(MonsterStats *enemy) final;
            void recalculateDerivedStats() final;
        private:
            friend class Inventory;

    };
}
#endif // CHARACTERSTATS_H
