#pragma once
#include "diabloexe/baseitem.h"
#include "itemenums.h"
#include "misc/enum_range.h"
#include <faworld/item/item.h>
#include <faworld/item/itembaseholder.h>
#include <functional>
#include <map>
#include <memory>
#include <random/random.h>
#include <vector>

namespace DiabloExe
{
    class DiabloExe;
}

namespace Cel
{
    class CelFile;
}

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace FAWorld
{
    class Item;
    enum class ItemId;
    enum class UniqueItemId;

    class BaseItemGenOptions
    {
    public:
        using thisType = BaseItemGenOptions;
    };

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
        std::unique_ptr<Item> generateBaseItem(ItemId id, const BaseItemGenOptions& options = {}) const;
        const ItemBase* randomItemBase(const ItemFilter::Callback& filter) const;
        const ItemPrefixOrSuffixBase* randomPrefixOrSuffixBase(const std::function<bool(const ItemPrefixOrSuffixBase&)>& filter) const;
        void applyRandomEnchantment(EquipmentItem& item, int32_t minLevel, int32_t maxLevel) const;

        void saveItem(const Item& item, FASaveGame::GameSaver& saver) const;
        std::unique_ptr<Item> loadItem(FASaveGame::GameLoader& loader) const;

        const ItemBaseHolder& getItemBaseHolder() const { return mItemBaseHolder; }

    private:
        const DiabloExe::ExeItem& getInfo(ItemId id) const;

        ItemBaseHolder mItemBaseHolder;
        std::map<int32_t, ItemId> mUniqueBaseItemIdToItemId;
        const DiabloExe::DiabloExe& mExe;
        Random::Rng& mRng;
    };
}
