#include "itemmanager.h"
#include <sstream>
#include <boost/bind.hpp>

#include <iostream>

namespace FAWorld
{
bool ItemManager::mIsLoaded = false;
std::map<uint8_t, Item> ItemManager::mRegisteredItems;
uint32_t ItemManager::lastUnique=0;
ItemManager::ItemManager()
{

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
            this->mRegisteredItems[this->mRegisteredItems.size()] = Item(it->second, mRegisteredItems.size());

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
void ItemManager::putItemOnFloor(Item& item, std::pair<size_t, size_t> floor_pos)
{

    std::map<ItemPosition, Item>::const_iterator it = std::find_if(
                mItemPositionMap.begin(),
                mItemPositionMap.end(),
                boost::bind(&std::map<ItemPosition, Item>::value_type::second, _1) == item
                );
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
                boost::bind(&std::map<ItemPosition, Item >::value_type::second, _1) == item
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
