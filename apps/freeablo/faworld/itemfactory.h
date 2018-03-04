#pragma once

#include <map>
#include <memory>
#include <vector>

#include "misc/enum_range.h"
#include "diabloexe/baseitem.h"
#include "misc/random.h"
#include "itemenums.h"

namespace DiabloExe
{
    class DiabloExe;
}

namespace Cel
{
    class CelFile;
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
        inline auto maxQLvl (int32_t value)
        {
            return [value](const DiabloExe::BaseItem &item) { return static_cast<int32_t> (item.qualityLevel) <= value; };
        }

        inline auto sellableGriswoldBasic ()
        {
            return [](const DiabloExe::BaseItem &item)
            {
                static const auto excludedTypes = {ItemType::misc, ItemType::gold, ItemType::staff, ItemType::ring, ItemType::amulet};
                return std::count (excludedTypes.begin (), excludedTypes.end (), static_cast<ItemType> (item.type)) == 0;
            };
        }
    };

    class ItemFactory
    {
    public:
        explicit ItemFactory(const DiabloExe::DiabloExe& exe);
        Item generateBaseItem(ItemId id, const BaseItemGenOptions& options = {}) const;
        Item generateUniqueItem(UniqueItemId id) const;
        template <typename... FilterTypes>
        ItemId randomItemId (const FilterTypes &... filters) const
        {
            static std::vector<ItemId> pool;
            pool.clear ();
            for (auto id : enum_range<ItemId> ())
                {
                    auto &info = getInfo (id);
                    bool filteredOut = false;
                    auto dummy = {(filteredOut = filteredOut || !filters(info),0)...};
                    if (filteredOut)
                        continue;
                    for (int32_t i = 0; i < static_cast<int32_t> (info.dropRate); ++i)
                         pool.push_back (id);
                }
            return pool[Random::randomInRange(0, pool.size () - 1)];
        }

    private:
        const DiabloExe::BaseItem &getInfo (ItemId id) const;
        // TODO: replace this with something more decent
        mutable std::unique_ptr<Cel::CelFile> mObjcursCel;
        std::map<int32_t, ItemId> mUniqueBaseItemIdToItemId;
        const DiabloExe::DiabloExe& mExe;
    };
}
