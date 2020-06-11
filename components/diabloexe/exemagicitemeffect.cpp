#include "exemagicitemeffect.h"
#include <misc/assert.h>
#include <sstream>
#include <string>

std::string exeMagicEffectTypeToString(ExeMagicEffectType type)
{
#define GENERATE(X)                                                                                                                                            \
    case ExeMagicEffectType::X:                                                                                                                                \
        return #X;

    switch (type)
    {
        GENERATE(PlusToHit)
        GENERATE(MinusToHit)
        GENERATE(PlusDamagePercent)
        GENERATE(MinusDamagePercent)
        GENERATE(PlusToHitAndDamagePercent)
        GENERATE(MinusToHitAndDamagePercent)
        GENERATE(PlusArmorPercent)
        GENERATE(MinusArmorPercent)
        GENERATE(PlusResistFire)
        GENERATE(PlusResistLightning)
        GENERATE(PlusResistMagic)
        GENERATE(PlusResistAll)
        GENERATE(PlusSpellLevels)
        GENERATE(PlusCharges)
        GENERATE(PlusDamageFire)
        GENERATE(PlusDamageLightning)
        GENERATE(PlusStrength)
        GENERATE(MinusStrength)
        GENERATE(PlusMagic)
        GENERATE(MinusMagic)
        GENERATE(PlusDexterity)
        GENERATE(MinusDexterity)
        GENERATE(PlusVitality)
        GENERATE(MinusVitality)
        GENERATE(PlusAllAttributes)
        GENERATE(MinusAllAttributes)
        GENERATE(PlusDamageTaken)
        GENERATE(MinusDamageTaken)
        GENERATE(PlusLife)
        GENERATE(MinusLife)
        GENERATE(PlusMana)
        GENERATE(MinusMana)
        GENERATE(PlusDurabilityPercent)
        GENERATE(CurseDurabilityPercent)
        GENERATE(Indestructible)
        GENERATE(PlusLightPercent)
        GENERATE(CurseLightPercent)
        GENERATE(MultipleArrows)
        GENERATE(PlusFireArrowDamage)
        GENERATE(PlusLightningArrowDamage)
        GENERATE(UniqueIcon)
        GENERATE(Deal1To3DamageToAttackers)
        GENERATE(ZeroMana)
        GENERATE(UserCantHeal)
        GENERATE(AbsorbHalfTrapDamage)
        GENERATE(Knockback)
        GENERATE(HitMonsterCantHeal)
        GENERATE(StealMana)
        GENERATE(StealLife)
        GENERATE(DamageArmor)
        GENERATE(FastAttack)
        GENERATE(FastHitRecovery)
        GENERATE(FastBlock)
        GENERATE(PlusDamage)
        GENERATE(RandomSpeedArrows)
        GENERATE(SetItemDamage)
        GENERATE(SetDurability)
        GENERATE(NoStrengthRequirement)
        GENERATE(SetCharges)
        GENERATE(FastAttack2)
        GENERATE(OneHanded)
        GENERATE(Plus200PercentOnDemons)
        GENERATE(AllResistancesZero)
        GENERATE(ConstantlyLoseLife)
        GENERATE(LifeSteal)
        GENERATE(Infravision)
        GENERATE(SetArmor)
        GENERATE(AddArmorToLife)
        GENERATE(Add10PercentOfManaToArmor)
        GENERATE(PlusLevelDependentResistFire)
        GENERATE(MinusArmor)
    }

    invalid_enum(ExeMagicEffectType, type);

#undef GENERATE
}

namespace DiabloExe
{
    ExeMagicItemEffect::ExeMagicItemEffect(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        // https://web.archive.org/web/20151015004713/http://www.thedark5.com/info/mod/mod1.html

        uint32_t nameTemp = exe.read32();

        mEffect = ExeMagicEffectType(exe.read32());
        mMinEffect = exe.read32();
        mMaxEffect = exe.read32();
        mQualLevel = exe.read32();
        mTargetTypesBitmask = MagicalItemTargetBitmask(exe.read32());
        mCompatibilityBitmask = CompatibilityBitMask(exe.read32());
        mDoubleProbabilityForPrefixes = bool(exe.read32());
        mNotCursed = bool(exe.read32());
        mMinGold = exe.read32();
        mMaxGold = exe.read32();
        mGoldMultiplier = exe.read32();

        mName = exe.readCStringFromWin32Binary(nameTemp, codeOffset);
        mIsPrefix = !mName.empty() && std::isupper(mName[0]);
    }

    std::string ExeMagicItemEffect::dump() const
    {
        std::stringstream ss;

        std::string targetTypeBitmaskString;
        {
            if (int32_t(mTargetTypesBitmask) & int32_t(MagicalItemTargetBitmask::Jewelery))
                ss << "Jewelery | ";
            if (int32_t(mTargetTypesBitmask) & int32_t(MagicalItemTargetBitmask::Bow))
                ss << "Bow | ";
            if (int32_t(mTargetTypesBitmask) & int32_t(MagicalItemTargetBitmask::Staff))
                ss << "Staff | ";
            if (int32_t(mTargetTypesBitmask) & int32_t(MagicalItemTargetBitmask::OtherWeapons))
                ss << "OtherWeapons | ";
            if (int32_t(mTargetTypesBitmask) & int32_t(MagicalItemTargetBitmask::Shield))
                ss << "Shield | ";
            if (int32_t(mTargetTypesBitmask) & int32_t(MagicalItemTargetBitmask::Armor))
                ss << "Armor | ";

            targetTypeBitmaskString = ss.str();
            if (targetTypeBitmaskString.empty())
                targetTypeBitmaskString = "None";
            else
                targetTypeBitmaskString = targetTypeBitmaskString.substr(0, targetTypeBitmaskString.size() - 3);
        }

        ss.str("");
        ss << "{" << std::endl
           << "\tmName: " << mName << std::endl
           << "\tmIdName: " << mIdName << std::endl
           << "\tmEffectType: " << (mIsPrefix ? "Prefix" : "Suffix") << std::endl
           << "\tmEffect: " << exeMagicEffectTypeToString(mEffect) << ", " << std::endl
           << "\tmMinEffect: " << mMinEffect << "," << std::endl
           << "\tmMaxEffect: " << mMaxEffect << "," << std::endl
           << "\tmQualLevel: " << mQualLevel << "," << std::endl
           << "\tmTargetTypeBitmask: " << targetTypeBitmaskString << "," << std::endl
           << "\tmCompatibilityBitmask: " << int32_t(mCompatibilityBitmask) << "," << std::endl
           << "\tmDoubleProbabilityForPrefixes: " << mDoubleProbabilityForPrefixes << "," << std::endl
           << "\tmNotCursed: " << mNotCursed << "," << std::endl
           << "\tmMinGold: " << mMinGold << "," << std::endl
           << "\tmMaxGold: " << mMaxGold << "," << std::endl
           << "\tmGoldMultiplier: " << mGoldMultiplier << "," << std::endl
           << "}" << std::endl;

        return ss.str();
    }
}
