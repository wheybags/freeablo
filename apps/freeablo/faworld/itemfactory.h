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
        using Callback = std::function<bool(const DiabloExe::BaseItem& item)>;
        Callback maxQLvl(int32_t value);
        Callback sellableGriswoldBasic();
    }

    class ItemFactory
    {
    public:
        explicit ItemFactory(const DiabloExe::DiabloExe& exe, Random::Rng& rng);
        Item generateBaseItem(ItemId id, const BaseItemGenOptions& options = {}) const;
        Item generateUniqueItem(UniqueItemId id) const;
        ItemId randomItemId(const ItemFilter::Callback& filter) const;

    private:
        const DiabloExe::BaseItem& getInfo(ItemId id) const;
        // TODO: replace this with something more decent
        mutable std::unique_ptr<Cel::CelFile> mObjcursCel;
        std::map<int32_t, ItemId> mUniqueBaseItemIdToItemId;
        const DiabloExe::DiabloExe& mExe;
        Random::Rng& mRng;
    };
}
