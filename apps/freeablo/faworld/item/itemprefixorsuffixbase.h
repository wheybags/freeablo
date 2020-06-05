#pragma once
#include <memory>
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

    class ItemPrefixOrSuffixBase
    {
    public:
        explicit ItemPrefixOrSuffixBase(const DiabloExe::ExeMagicItemEffect& exeEffect);
        ~ItemPrefixOrSuffixBase();

        std::unique_ptr<ItemPrefixOrSuffix> create() const;

    public:
        std::string mName;
        bool mCursed = false;
        bool mIsPrefix = false;

        std::vector<std::unique_ptr<MagicEffectBase>> mEffects;
    };
}
