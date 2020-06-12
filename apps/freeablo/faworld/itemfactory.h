#pragma once
#include <faworld/item/item.h>
#include <faworld/item/itembaseholder.h>
#include <functional>
#include <memory>
#include <random/random.h>

namespace DiabloExe
{
    class DiabloExe;
}

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace FAWorld
{
    namespace ItemFilter
    {
        using Callback = std::function<bool(const ItemBase& base)>;
        Callback maxQLvl(int32_t value);
        Callback sellableGriswoldBasic();
    }

    class ItemFactory
    {
    public:
        explicit ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng);
        std::unique_ptr<Item> generateBaseItem(const std::string& id) const;
        const ItemBase* randomItemBase(const ItemFilter::Callback& filter) const;
        const ItemPrefixOrSuffixBase* randomPrefixOrSuffixBase(const std::function<bool(const ItemPrefixOrSuffixBase&)>& filter) const;
        void applyRandomEnchantment(EquipmentItem& item, int32_t minLevel, int32_t maxLevel) const;

        void saveItem(const Item& item, FASaveGame::GameSaver& saver) const;
        std::unique_ptr<Item> loadItem(FASaveGame::GameLoader& loader) const;

        const ItemBaseHolder& getItemBaseHolder() const { return mItemBaseHolder; }

    private:
        ItemBaseHolder mItemBaseHolder;
        const DiabloExe::DiabloExe& mExe;
        Random::Rng& mRng;
    };
}
