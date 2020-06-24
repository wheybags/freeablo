#include "itemprefixorsuffix.h"
#include "itemprefixorsuffixbase.h"
#include <fasavegame/gameloader.h>
#include <faworld/magiceffects/magiceffect.h>
#include <faworld/magiceffects/magiceffectbase.h>

namespace FAWorld
{
    ItemPrefixOrSuffix::ItemPrefixOrSuffix(const ItemPrefixOrSuffixBase* base) : mBase(base) {}

    void ItemPrefixOrSuffix::init()
    {
        for (const auto& effectBase : getBase()->mEffects)
        {
            mEffects.emplace_back(effectBase->create());
            mEffects.back()->init();
        }
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

    void ItemPrefixOrSuffix::save(FASaveGame::GameSaver& saver) const
    {
        for (const auto& effect : mEffects)
            effect->save(saver);
    }

    void ItemPrefixOrSuffix::load(FASaveGame::GameLoader& loader)
    {
        // TODO: deal with changing effects here, once we add mod support

        mEffects.reserve(getBase()->mEffects.size());
        for (const auto& effectBase : getBase()->mEffects)
        {
            std::unique_ptr<MagicEffect> effect = effectBase->create();
            effect->load(loader);
            mEffects.emplace_back(std::move(effect));
        }
    }

    ItemPrefixOrSuffix::~ItemPrefixOrSuffix() = default;
}
