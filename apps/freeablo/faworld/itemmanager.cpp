#include "itemmanager.h"
#include <sstream>
#include <iostream>
#include <unordered_map>

namespace FAWorld
{
bool ItemManager::mIsLoaded = false;
std::map<uint8_t, Item> ItemManager::mRegisteredItems;
std::map<uint8_t, Item> ItemManager::mUniqueItems;
std::map<uint32_t, DiabloExe::BaseItem> ItemManager::mUniqueCodeToBaseItem;
std::unordered_map<std::string, const Item *> mItemByName; // of both types

uint32_t ItemManager::lastUnique=0;
ItemManager::ItemManager()
{

}

ItemManager& ItemManager::get()
{
    static ItemManager instance;
    return instance;
}

uint32_t ItemManager::getUniqueItemCode()
{
    return ++lastUnique;
}

void ItemManager::loadItems(DiabloExe::DiabloExe * exe)
{

    if(!mIsLoaded && exe != NULL)
    {
        std::map<std::string, DiabloExe::BaseItem> itemMap = exe->getItemMap();
        for(std::map<std::string, DiabloExe::BaseItem>::const_iterator it = itemMap.begin();it !=itemMap.end();++it)
        {
            auto &item = this->mRegisteredItems[static_cast<uint8_t> (this->mRegisteredItems.size())];
            item = Item(it->second, mRegisteredItems.size());
            mItemByName[item.getName()] = &item;
            if(it->second.uniqCode !=0)
                this->mUniqueCodeToBaseItem[it->second.uniqCode] = it->second;
        }

    }
}

void ItemManager::loadUniqueItems(DiabloExe::DiabloExe * exe)
{
    if(!mIsLoaded && exe != NULL)
    {
        const std::map<std::string, DiabloExe::UniqueItem>& uniqueItemMap = exe->getUniqueItemMap();
        for(std::map<std::string, DiabloExe::UniqueItem>::const_iterator it = uniqueItemMap.begin();it !=uniqueItemMap.end();++it)
        {
            auto &item = this->mUniqueItems[static_cast<uint8_t> (this->mUniqueItems.size())];
            item = Item(it->second, mUniqueItems.size());
            mItemByName[item.getName()] = &item;
        }
        mIsLoaded=true;
    }

}

void ItemManager::addItem(Item &item, std::pair<size_t, size_t> floorPosition, uint32_t count)
{
    item.setCount(count);
    ItemPosition temp = ItemPosition(floorPosition);
    mItemPositionMap[temp]=item;

    return;

}

void ItemManager::dumpBaseItems() const
{
    std::stringstream ss;
    ss << "{" << std::endl;
    for(std::map<uint8_t, Item >::const_iterator it=mRegisteredItems.begin();it != mRegisteredItems.end();++it)
    {
        ss << "\t" << (size_t) it->first << ": \t" << it->second.getName() << std::endl;

    }
    ss << "\tCount:\t" << (size_t)mRegisteredItems.size() << std::endl;
    ss << "}" << std::endl;
    std::cout << ss.str();
}

void ItemManager::dumpUniqueItems() const
{
    std::stringstream ss;
    ss << "{" << std::endl;
    for(std::map<uint8_t, Item >::const_iterator it=mUniqueItems.begin();it != mUniqueItems.end();++it)
    {
        ss << "\t" << (size_t) it->first << ": \t" << it->second.getName() << std::endl;

    }
    ss << "\tCount:\t" << (size_t)mUniqueItems.size() << std::endl;
    ss << "}" << std::endl;
    std::cout << ss.str();
}

void ItemManager::dumpItemPositions()
{

    std::stringstream ss;
    ss << "{" << std::endl;
    for(std::map<ItemPosition, Item >::const_iterator it=mItemPositionMap.begin();it != mItemPositionMap.end();++it)
    {
        ss << "\t" << (size_t) it->second.getUniqueId() << ": \t" << it->second.getName() << std::endl;

    }
    ss << "\tCount:\t" << (size_t)mItemPositionMap.size() << std::endl;
    ss << "}" << std::endl;
    std::cout << ss.str();

}


Item ItemManager::getBaseItem(uint8_t id) const
{
    uint32_t uniqueId = getUniqueItemCode();
    Item search = mRegisteredItems.find(id)->second;
    search.setUniqueId(uniqueId);
    return search;
}

Item ItemManager::getUniqueItem(uint8_t id) const
{
    uint32_t uniqueId = getUniqueItemCode();
    Item search = mUniqueItems.find(id)->second;
    search.setUniqueId(uniqueId);
    return search;

}

Item ItemManager::getItemByName(const std::string& name)
{
    auto it = mItemByName.find(name);
    if (it != mItemByName.end ())
        return *it->second;

    assert (false); // Item not found
    return {};
}

    DiabloExe::BaseItem & ItemManager::getBaseItemByUniqueCode(uint8_t uniqCode)
{
    return mUniqueCodeToBaseItem.find(uniqCode)->second;


}

void ItemManager::putItemOnFloor(Item& item, std::pair<size_t, size_t> floor_pos)
{

    std::map<ItemPosition, Item>::const_iterator it = std::find_if(
                mItemPositionMap.begin(),
                mItemPositionMap.end(),
                [&](const std::pair<ItemPosition, Item> & pair)->bool {
                    return pair.second == item;
                });

    ItemPosition pos = it->first;
    if(it != mItemPositionMap.end())
    {
        pos.setFloorPosition(floor_pos);
    }
    else
    {
        ItemPosition new_pos;
        new_pos.setFloorPosition(floor_pos);
        mItemPositionMap[new_pos] = item;

    }
    return;




}

void ItemManager::removeItem(Item item)
{

    std::map<ItemPosition, Item>::const_iterator it = std::find_if(
                mItemPositionMap.begin(),
                mItemPositionMap.end(),
                [&](const std::pair<ItemPosition, Item> & pair)->bool {
                    return pair.second == item;
                }
    );

    if(it != mItemPositionMap.end())
    {
        mItemPositionMap.erase(it->first);

    }

    return;


}


std::pair<size_t,size_t> ItemPosition::getFloorPosition() const
{
    return this->mFloorPosition;

}

void ItemPosition::setFloorPosition(std::pair<size_t,size_t> pos)
{
    this->mFloorPosition = pos;
}


ItemPosition::ItemPosition(std::pair<size_t,size_t> floorPosition)
{
    this->mFloorPosition = floorPosition;
}

bool ItemPosition::operator<(const ItemPosition rhs) const
{
    return mFloorPosition < rhs.mFloorPosition;

}

bool ItemPosition::operator ==(const ItemPosition rhs) const
{
    if(mFloorPosition != rhs.mFloorPosition)
        return false;

    return true;
}


}
