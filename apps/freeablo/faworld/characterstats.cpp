#include "characterstats.h"

namespace FAWorld
{
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
    mArmourClass += mDexterity/5;
    mDamageDoneBow += (mStrength * mLevel)/200;
    mDamageDoneMelee += (mStrength * mLevel)/100.0;
    mBlockingChance += mBlockingBonus;
    mBlockingChancePVP += mBlockingBonus;
    mBlockingChanceTrap += mDexterity + mBlockingBonus;
    mDamageDoneMelee += mActor->mInventory.getTotalAttackDamage();
    mDamageDoneBow   += mActor->mInventory.getTotalAttackDamage();
    mArmourClass += mActor->mInventory.getTotalArmourClass();
    mChanceToHitArrow += 10;

    //printStats();

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
    mArmourClass += mDexterity/5;
    mDamageDoneBow += ((mStrength + mDexterity) * mLevel)/100;
    mDamageDoneMelee += ((mStrength + mDexterity) * mLevel)/200.0;
    mBlockingChance += mBlockingBonus;
    mBlockingChancePVP += mBlockingBonus;
    mBlockingChanceTrap += mDexterity + mBlockingBonus;
    mDamageDoneMelee += mActor->mInventory.getTotalAttackDamage();
    mDamageDoneBow   += mActor->mInventory.getTotalAttackDamage();
    mArmourClass += mActor->mInventory.getTotalArmourClass();
    mChanceToHitArrow += 20;

    printStats();

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
    mArmourClass += mDexterity/5;
    mDamageDoneBow += (mStrength * mLevel)/200;
    mDamageDoneMelee += (mStrength * mLevel)/100.0;
    mBlockingChance += mBlockingBonus;
    mBlockingChancePVP += mBlockingBonus;
    mBlockingChanceTrap += mDexterity + mBlockingBonus;
    mDamageDoneMelee += mActor->mInventory.getTotalAttackDamage();
    mDamageDoneBow   += mActor->mInventory.getTotalAttackDamage();
    mArmourClass += mActor->mInventory.getTotalArmourClass();
    mChanceToHitArrow += 10;

    printStats();

}

}
