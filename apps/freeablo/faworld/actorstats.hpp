#ifndef ACTORSTATS_H
#define ACTORSTATS_H
#include <stdint.h>
#include <vector>

#include <diabloexe/diabloexe.h>
#include "../falevelgen/random.h"
#include "actor.h"
#include "item.h"

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
            virtual void processEffects()
            {
                std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> effects = mActor->mInventory.getTotalEffects();
                printf("%lu -- size of effects\n", effects.size());

                for(std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>>::iterator it= effects.begin();it != effects.end(); ++it)
                {

                    Item::ItemEffect effect;
                    uint32_t minRange, maxRange, id;
                    std::tie(effect, minRange, maxRange, id) = *it;
                    uint32_t range = 0;
                    range = FALevelGen::randomInRange(minRange, maxRange);
                    switch(effect)
                    {
                        case Item::IncPercentChanceToHit:
                            mChanceToHitCharacterScreen += range;
                            mChanceToHitMelee += range;
                            mChanceToHitArrow += range;
                            break;

                        case Item::DecPercentChanceToHit:
                            mChanceToHitCharacterScreen -= range;
                            mChanceToHitMelee -= range;
                            mChanceToHitArrow -= range;
                            break;

                        case Item::IncPercentDamageDone:
                            mDamageDoneBow += range;
                            mDamageDoneMelee += range;
                            break;

                        case Item::DecPercentDamageDone:
                            mDamageDoneBow -= range;
                            mDamageDoneMelee -= range;
                            break;

                        case Item::IncPercentDamageDoneChanceToHit:
                            mDamageDoneBow += range;
                            mDamageDoneMelee += range;
                            mChanceToHitCharacterScreen += range;
                            mChanceToHitMelee += range;
                            mChanceToHitArrow += range;

                            break;

                        case Item::DecPercentDamageDoneChanceToHit:
                            mDamageDoneBow -= range;
                            mDamageDoneMelee -= range;
                            mChanceToHitCharacterScreen -= range;
                            mChanceToHitMelee -= range;
                            mChanceToHitArrow -= range;
                            break;

                        case Item::IncPercentArmourClass:
                            mArmourClass += range;
                            break;

                        case Item::DecPercentArmourClass:
                            mArmourClass -= range;
                            break;

                        case Item::IncPercentResistFire:
                            mResistanceFire += range;
                            break;

                        case Item::IncPercentResistLightning:
                            mResistanceLightning += range;
                            break;

                        case Item::IncPercentResistMagic:
                            mResistanceMagic += range;
                            break;

                        case Item::IncPercentResistAll:
                            mResistanceLightning += range;
                            mResistanceMagic += range;
                            mResistanceFire += range;
                            break;

                        case Item::ModSpellLevel:
                            break;

                        case Item::IncCharges:
                            break;

                        case Item::IncFireDamage:
                            break;

                        case Item::IncLightningDamage:
                            break;

                        case Item::IncStrength:
                            mBonusStrength += range;
                            break;

                        case Item::DecStrength:
                            mBonusStrength -= range;
                            break;

                        case Item::IncMagic:
                            mBonusMagic += range;
                            break;

                        case Item::DecMagic:
                            mBonusMagic -= range;
                            break;

                        case Item::IncDexterity:
                            mBonusDexterity += range;
                            break;

                        case Item::DecDexterity:
                            mBonusDexterity -= range;
                            break;

                        case Item::IncVitality:
                            mBonusVitality +=  range;
                            break;

                        case Item::DecVitality:
                            mBonusVitality -= range;
                            break;

                        case Item::IncAllBasicStats:
                            mBonusMagic += range;
                            mBonusVitality += range;
                            mBonusDexterity += range;
                            mBonusStrength += range;
                            break;

                        case Item::DecAllBasicStats:
                            mBonusMagic -= range;
                            mBonusVitality -= range;
                            mBonusDexterity -= range;
                            mBonusStrength -= range;
                            break;

                        case Item::IncDamageTaken:
                            mDamageTakenMultiplier += range;

                            break;

                        case Item::DecDamageTaken:
                            mDamageTakenMultiplier -= range;
                            break;

                        case Item::IncHP:
                            mHP += range;
                            break;

                        case Item::DecHP:
                            mHP -= range;
                            break;

                        case Item::IncMana:
                            mMana += range;
                            break;

                        case Item::DecMana:
                            mMana -= range;
                            break;
                        case Item::IncPercentDurability:
                            break;

                        case Item::DecPercentDurability:
                            break;

                        case Item::Indestructible:
                            break;

                        case Item::IncLightRadius:
                            mLightRadius += range;
                            break;

                        case Item::DecLightRadius:
                            mLightRadius += range;
                            break;

                        case Item::Unknown0:
                            break;

                        case Item::MultipleArrows:
                            break;

                        case Item::IncPercentFireArrowDamage:
                            break;

                        case Item::IncPercentLightningArrowDamage:
                            break;

                        case Item::UniquePicture:
                            break;

                        case Item::Thorns:
                            break;

                        case Item::AllMana:
                            break;

                        case Item::PlayerNoHeal:
                            break;

                        case Item::Unknown1:
                            break;

                        case Item::Unknown2:
                            break;

                        case Item::Unknown3:
                            break;

                        case Item::Unknown4:
                            break;

                        case Item::HalfTrapDamage:
                            mHasThieves = true;
                            break;

                        case Item::Knockback:
                            break;

                        case Item::MonsterNoHeal:
                            break;

                        case Item::PercentManaSteal:
                            break;

                        case Item::PercentLifeSteal:
                            break;

                        case Item::ArmourPenetration:
                            break;

                        case Item::AttackSpeed0:
                            break;

                        case Item::HitRecovery:
                            break;

                        case Item::FastBlock:
                            break;

                        case Item::IncDamageDone:
                            break;

                        case Item::RandomArrowSpeed:
                            break;

                        case Item::UnusualDamage:
                            break;

                        case Item::AlteredDurability:
                            break;

                        case Item::NoStrengthRequirment:
                            break;

                        case Item::Spell:
                            break;

                        case Item::AttackSpeed1:
                            break;

                        case Item::OneHanded:
                            break;

                        case Item::AntiDemon:
                            break;

                        case Item::ZeroAllResist:
                            break;

                        case Item::Unknown5:
                            break;

                        case Item::ConstantLifeDrain:
                            break;

                        case Item::PercentFixedLifeSteal:
                            break;

                        case Item::Infravision:
                            break;

                        case Item::SpecifiedArmourClass:
                            break;

                        case Item::IncHPWithArmourClass:
                            break;

                        case Item::IncArmourClassWithMana:
                            break;

                        case Item::IncFireResistWithLevel:
                            break;

                        case Item::DecArmourClass:
                            break;
                        default:
                            break;

                    }
                }
                printf("Dexterity at ProcessEffects: %u\n", mStartingDexterity);
                mChanceToHitCharacterScreen += 50 + mStartingDexterity/2;
                mChanceToHitMelee += mChanceToHitCharacterScreen;
                mChanceToHitArrow += mChanceToHitCharacterScreen + mDexterity/2 ;
                mChanceToHitMagic += 50 + mMagic;


            }

            virtual void printStats()
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
            const std::vector<uint32_t> mExpForLevel;
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
            Actor * mActor;
    };
}



#endif //ACTORSTATS_H
