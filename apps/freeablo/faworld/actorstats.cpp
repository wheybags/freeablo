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
    void ActorStats::processEffects()
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
        mChanceToHitCharacterScreen += 50 + mStartingDexterity/2;
        mChanceToHitMelee += mChanceToHitCharacterScreen;
        mChanceToHitArrow += mChanceToHitCharacterScreen + mDexterity/2 ;
        mChanceToHitMagic += 50 + mMagic;
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
        if (!(mCurrentHP <= 0))
        {
            mActor->setAnimation(AnimState::hit);
            mActor->mAnimPlaying = true;
        }
        else
            mActor->mAnimPlaying = false;


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
