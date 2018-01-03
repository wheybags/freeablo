#pragma once

#include <map>
#include <memory>

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

    class ItemFactory
    {
    public:
        explicit ItemFactory(const DiabloExe::DiabloExe& exe);
        Item generateBaseItem(ItemId id, const BaseItemGenOptions &options = {}) const;
        Item generateUniqueItem(UniqueItemId id) const;

    private:
        // TODO: replace this with something more decent
        mutable std::unique_ptr<Cel::CelFile> mObjcursCel;
        std::map<int32_t, ItemId> mUniqueBaseItemIdToItemId;
        const DiabloExe::DiabloExe& mExe;
    };
}
