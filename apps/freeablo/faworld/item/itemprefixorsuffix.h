#pragma once
#include <memory>
#include <vector>

namespace FAWorld
{
    class ItemPrefixOrSuffixBase;
    class MagicEffect;

    class ItemPrefixOrSuffix
    {
    public:
        explicit ItemPrefixOrSuffix(const ItemPrefixOrSuffixBase* base);
        void init();
        ~ItemPrefixOrSuffix();

        const ItemPrefixOrSuffixBase* getBase() { return mBase; }

    private:
        const ItemPrefixOrSuffixBase* mBase = nullptr;
        std::vector<std::unique_ptr<MagicEffect>> mEffects;
    };
}
