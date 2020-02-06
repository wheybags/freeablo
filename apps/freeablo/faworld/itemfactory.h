#pragma once
#include "diabloexe/baseitem.h"
#include "itemenums.h"
#include "misc/enum_range.h"
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
        std::function<bool(const DiabloExe::BaseItem& item)> maxQLvl(int32_t value);
        std::function<bool(const DiabloExe::BaseItem& item)> sellableGriswoldBasic();
    }

    class ItemFactory
    {
    public:
        explicit ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng);
        Item generateBaseItem(ItemId id, const BaseItemGenOptions& options = {}) const;
        Item generateUniqueItem(UniqueItemId id) const;
        template <typename... FilterTypes> ItemId randomItemId(const FilterTypes&... filters) const
        {
            static std::vector<ItemId> pool;
            pool.clear();
            for (auto id : enum_range<ItemId>())
            {
                const DiabloExe::BaseItem& info = getInfo(id);
                bool filteredOut = false;
                static_cast<void>(std::initializer_list<int>{(filteredOut = filteredOut || !filters(info), 0)...});
                if (filteredOut)
                    continue;
                for (int32_t i = 0; i < static_cast<int32_t>(info.dropRate); ++i)
                    pool.push_back(id);
            }
            return pool[mRng.randomInRange(0, pool.size() - 1)];
        }

    private:
        const DiabloExe::BaseItem& getInfo(ItemId id) const;
        // TODO: replace this with something more decent
        mutable std::unique_ptr<Cel::CelFile> mObjcursCel;
        std::map<int32_t, ItemId> mUniqueBaseItemIdToItemId;
        const DiabloExe::DiabloExe& mExe;
        Random::Rng& mRng;
    };
}
