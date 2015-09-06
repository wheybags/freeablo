#include "characterstats.h"
#include "player.h"
#include "../falevelgen/random.h"
namespace FAWorld
{

    void CharacterStatsBase::processEffects()
    {

        std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> effects;
        effects = mPlayer->mInventory.getTotalEffects();
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
                break;

                case Item::DecPercentDamageDone:
                break;

                case Item::IncPercentDamageDoneChanceToHit:
                    mChanceToHitCharacterScreen += range;
                    mChanceToHitMelee += range;
                    mChanceToHitArrow += range;
                break;

                case Item::DecPercentDamageDoneChanceToHit:
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

    uint32_t CharacterStatsBase::getDamage()
    {
        return 0;

    }


    void MeleeStats::recalculateDerivedStats()
    {
        clearDerivedStats();
        processEffects();

        mStrength += mStartingStrength + mBonusStrength + mSpentLevelsOnStrength;
        mMagic += mStartingMagic + mBonusMagic + mSpentLevelsOnMagic;
        mDexterity += mStartingDexterity + mBonusDexterity + mSpentLevelsOnDexterity;
        mVitality += mStartingVitality + mBonusVitality + mSpentLevelsOnVitality;
        mHP += 2*mVitality + 2*mBonusVitality + 2*mLevel + 18;
        mMana += mMagic + mLevel -1;
        mBlockingChance += mBlockingBonus;
        mBlockingChancePVP += mBlockingBonus;
        mBlockingChanceTrap += mDexterity + mBlockingBonus;        

        mChanceToHitArrow += 10;
    }

    void RangerStats::recalculateDerivedStats()
    {
        clearDerivedStats();
        processEffects();

        mStrength += mStartingStrength  + mSpentLevelsOnStrength;
        mMagic += mStartingMagic + mSpentLevelsOnMagic;
        mDexterity += mStartingDexterity + mSpentLevelsOnDexterity;
        mVitality += mStartingVitality + mSpentLevelsOnVitality;
        mHP += mVitality + 1.5*mBonusVitality + 2*mLevel + 23;
        mMana += mMagic + 1.5*mBonusMagic + 2*mLevel + 5;
        mBlockingChance += mBlockingBonus;
        mBlockingChancePVP += mBlockingBonus;
        mBlockingChanceTrap += mDexterity + mBlockingBonus;
        mChanceToHitArrow += 20;
    }

    void MageStats::recalculateDerivedStats()
    {
        clearDerivedStats();
        processEffects();

        mStrength += mStartingStrength + mBonusStrength + mSpentLevelsOnStrength;
        mMagic += mStartingMagic + mBonusMagic + mSpentLevelsOnMagic;
        mDexterity += mStartingDexterity + mBonusDexterity + mSpentLevelsOnDexterity;
        mVitality += mStartingVitality + mBonusVitality + mSpentLevelsOnVitality;
        mHP += mVitality + mBonusVitality + mLevel + 9;
        mMana += 2*mMagic + 2*mBonusMagic + 2*mLevel -2;
        mBlockingChance += mBlockingBonus;
        mBlockingChancePVP += mBlockingBonus;
        mBlockingChanceTrap += mDexterity + mBlockingBonus;
        mChanceToHitArrow += 10;
    }

}
