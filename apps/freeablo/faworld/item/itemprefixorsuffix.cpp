#include "itemprefixorsuffix.h"
#include "itemprefixorsuffixbase.h"
#include <faworld/magiceffects/magiceffect.h>
#include <faworld/magiceffects/magiceffectbase.h>

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

    ItemPrefixOrSuffix::~ItemPrefixOrSuffix() = default;
}
