#pragma once
#include "itembase.h"
#include "itemprefixorsuffixbase.h"
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

        std::unique_ptr<Item> createItem(const std::string& baseTypeId) const;
        const ItemBase* getItemBase(const std::string& key) const { return mAllItemBases.at(key).get(); }

        const ItemPrefixOrSuffixBase* getItemPrefixOrSuffixBase(const std::string& key) const { return mAllItemPrefixSuffixBases.at(key).get(); }

        const std::unordered_map<std::string, std::unique_ptr<ItemBase>>& getAllItemBases() const { return mAllItemBases; }
        const std::unordered_map<std::string, std::unique_ptr<ItemPrefixOrSuffixBase>>& getAllItemPrefixSuffixBases() const
        {
            return mAllItemPrefixSuffixBases;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<ItemBase>> mAllItemBases;
        std::unordered_map<std::string, std::unique_ptr<ItemPrefixOrSuffixBase>> mAllItemPrefixSuffixBases;
    };
}
