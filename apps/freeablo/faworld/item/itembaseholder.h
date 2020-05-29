#pragma once
#include "itembase.h"
#include <memory>
#include <unordered_map>

namespace DiabloExe
{
    class DiabloExe;
}

namespace FAWorld
{
    class ItemBaseHolder
    {
    public:
        explicit ItemBaseHolder(const DiabloExe::DiabloExe& exe);

        std::unique_ptr<Item2> createItem(const std::string& baseTypeId) const;
        const ItemBase* get(const std::string& key) const { return mAllItemBases.at(key).get(); }

    private:
        std::unordered_map<std::string, std::unique_ptr<ItemBase>> mAllItemBases;
    };
}
