#include "itemprefixorsuffix.h"
#include "itemprefixorsuffixbase.h"
#include <faworld/magiceffects/magiceffect.h>
#include <faworld/magiceffects/magiceffectbase.h>
#include <fmt/format.h>

namespace FAWorld
{
    ItemPrefixOrSuffix::ItemPrefixOrSuffix(const ItemPrefixOrSuffixBase* base) : mBase(base)
    {
        for (const auto& effectBase : mBase->mEffects)
            mEffects.push_back(effectBase->create());
    }

    void ItemPrefixOrSuffix::init()
    {
        for (auto& effect : mEffects)
            effect->init();
    }

    std::string ItemPrefixOrSuffix::getFullDescription() const
    {
        std::string description;
        for (const auto& effect : mEffects)
            description += effect->getFullDescription() + "\n";

        return description;
    }

    void ItemPrefixOrSuffix::apply(MagicStatModifiers& modifiers) const
    {
        for (const auto& effect : mEffects)
            effect->apply(modifiers);
    }

    ItemPrefixOrSuffix::~ItemPrefixOrSuffix() = default;
}
