#pragma once
#include <memory>
#include <misc/commonenums.h>
#include <string>
#include <vector>

namespace DiabloExe
{
    class ExeMagicItemEffect;
}

namespace FAWorld
{
    class MagicEffectBase;
    class ItemPrefixOrSuffix;
    class EquipmentItem;

    class ItemPrefixOrSuffixBase
    {
    public:
        explicit ItemPrefixOrSuffixBase(const DiabloExe::ExeMagicItemEffect& exeEffect);
        ~ItemPrefixOrSuffixBase();

        std::unique_ptr<ItemPrefixOrSuffix> create() const;
        bool canBeAppliedTo(const EquipmentItem& item) const;

    public:
        std::string mId;

        std::string mName;
        bool mCursed = false;
        bool mIsPrefix = false;
        int32_t mQuality = 0;
        MagicalItemTargetBitmask mTargetTypesBitmask = MagicalItemTargetBitmask::None;
        int32_t mDropRate = 1;

        std::vector<std::unique_ptr<MagicEffectBase>> mEffects;
    };
}
