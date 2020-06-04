#pragma once
#include <cstdint>
#include <faio/fafileobject.h>
#include <map>
#include <string>

enum class MagicalItemType
{
    None = 0x0,
    Jewelery = 0x1,
    Bow = 0x10,
    Staff = 0x100,
    OtherWeapons = 0x1000,
    Shield = 0x10000,
    Armor = 0x100000,
};

enum class MagicalEffectType
{
    Prefix,
    Suffix,
};

namespace DiabloExe
{
    class ExeMagicItemEffect
    {
    public:
        std::string mName;
        MagicalEffectType mEffectType = {};

        int32_t mEffect = 0;
        int32_t mMinEffect = 0;
        int32_t mMaxEffect = 0;
        int32_t mQualLevel = 0;
        MagicalItemType mTargetTypeBitmask = MagicalItemType::None;

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
