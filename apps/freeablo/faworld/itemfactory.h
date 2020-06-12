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
    using ItemFilter = std::function<bool(const ItemBase& base)>;
    using ItemPrefixOrSuffixFilter = std::function<bool(const ItemPrefixOrSuffixBase&)>;

    class ItemFactory
    {
    public:
        explicit ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng);

        std::unique_ptr<Item> generateBaseItem(const std::string& id) const;

        enum class ItemGenerationType
        {
            Normal,
            OnlyBaseItems,
            AlwaysMagical,
        };
        std::unique_ptr<Item> generateRandomItem(int32_t itemLevel, ItemGenerationType generationType) const;
        std::unique_ptr<Item> generateRandomItem(int32_t itemLevel, ItemGenerationType generationType, const ItemFilter& filter) const;

        const ItemBase* randomItemBase(const ItemFilter& filter) const;
        const ItemPrefixOrSuffixBase* randomPrefixOrSuffixBase(const ItemPrefixOrSuffixFilter& filter) const;
        void applyRandomEnchantment(EquipmentItem& item, int32_t minLevel, int32_t maxLevel) const;

        void saveItem(const Item& item, FASaveGame::GameSaver& saver) const;
        std::unique_ptr<Item> loadItem(FASaveGame::GameLoader& loader) const;

        const ItemBaseHolder& getItemBaseHolder() const { return mItemBaseHolder; }

    private:
        ItemBaseHolder mItemBaseHolder;
        Random::Rng& mRng;
    };
}
