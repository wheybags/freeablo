#pragma once
#include <cstdint>
#include <faio/fafileobject.h>
#include <map>
#include <misc/commonenums.h>
#include <string>

enum class ExeMagicEffectType
{
    PlusToHit = 0x00,
    MinusToHit = 0x01,
    PlusDamagePercent = 0x02,
    MinusDamagePercent = 0x03,
    PlusToHitAndDamagePercent = 0x04,
    MinusToHitAndDamagePercent = 0x05,
    PlusArmorPercent = 0x06,
    MinusArmorPercent = 0x07,
    PlusResistFire = 0x08,
    PlusResistLightning = 0x09,
    PlusResistMagic = 0x0A,
    PlusResistAll = 0x0B,
    PlusSpellLevels = 0x0E,
    PlusCharges = 0x0F,
    PlusDamageFire = 0x10,
    PlusDamageLightning = 0x11,
    PlusStrength = 0x13,
    MinusStrength = 0x14,
    PlusMagic = 0x15,
    MinusMagic = 0x16,
    PlusDexterity = 0x17,
    MinusDexterity = 0x18,
    PlusVitality = 0x19,
    MinusVitality = 0x1A,
    PlusAllAttributes = 0x1B,
    MinusAllAttributes = 0x1C,
    PlusDamageTaken = 0x1D,
    MinusDamageTaken = 0x1E,
    PlusLife = 0x1F,
    MinusLife = 0x20,
    PlusMana = 0x21,
    MinusMana = 0x22,
    PlusDurabilityPercent = 0x23,
    CurseDurabilityPercent = 0x24,
    Indestructible = 0x25,
    PlusLightPercent = 0x26,
    CurseLightPercent = 0x27,
    // Unused = 0x28,
    MultipleArrows = 0x29,
    PlusFireArrowDamage = 0x2A,
    PlusLightningArrowDamage = 0x2B,
    UniqueIcon = 0x2C,
    Deal1To3DamageToAttackers = 0x2D,
    ZeroMana = 0x2E,
    UserCantHeal = 0x2F,
    // Unused = 0x30,
    // Unused = 0x31,
    // Unused = 0x32,
    // Unused = 0x33,
    AbsorbHalfTrapDamage = 0x34,
    Knockback = 0x35,
    HitMonsterCantHeal = 0x36,
    StealMana = 0x37,
    StealLife = 0x38,
    DamageArmor = 0x39,
    FastAttack = 0x3A,
    FastHitRecovery = 0x3B,
    FastBlock = 0x3C,
    PlusDamage = 0x3D,
    RandomSpeedArrows = 0x3E,
    SetItemDamage = 0x3F,
    SetDurability = 0x40,
    NoStrengthRequirement = 0x41,
    SetCharges = 0x42,
    FastAttack2 = 0x43,
    OneHanded = 0x44,
    Plus200PercentOnDemons = 0x45,
    AllResistancesZero = 0x46,
    // Unused = 0x47,
    ConstantlyLoseLife = 0x48,
    LifeSteal = 0x49,
    Infravision = 0x4A,
    SetArmor = 0x4B,
    AddArmorToLife = 0x4C,
    Add10PercentOfManaToArmor = 0x4D,
    PlusLevelDependentResistFire = 0x4E,
    MinusArmor = 0x4F,
};

std::string exeMagicEffectTypeToString(ExeMagicEffectType type);

namespace DiabloExe
{
    class ExeMagicItemEffect
    {
    public:
        std::string mName;
        std::string mIdName;
        bool mIsPrefix = false;

        ExeMagicEffectType mEffect = {};
        int32_t mMinEffect = 0;
        int32_t mMaxEffect = 0;
        int32_t mQualLevel = 0;
        MagicalItemTargetBitmask mTargetTypesBitmask = MagicalItemTargetBitmask::None;

        enum class CompatibilityBitMask
        {
            All = 0,
            A = 1, // I really can't tell what these should be called
            B = 16,
        };

        // This determines the effects compatibility with other effects.
        // eg: you can combine an A prefix with an All suffix, or an A suffix,
        // but you cannot combine an A prefix with a B prefix, or vice versa.
        CompatibilityBitMask mCompatibilityBitmask = CompatibilityBitMask::All;

        bool mDoubleProbabilityForPrefixes = false;
        bool mNotCursed = false;
        int mMinGold = 0;
        int mMaxGold = 0;
        int mGoldMultiplier = 0;

        std::string dump() const;
        ExeMagicItemEffect() = default;

    private:
        ExeMagicItemEffect(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;
    };
}
