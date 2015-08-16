#include "itemmanager.h"
#include <sstream>
#include <iostream>

namespace Level
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

void ItemManager::addItem(Item &item,
                          Item::equipLoc invType,
                          std::pair<size_t,size_t> * floorPosition,
                          std::pair<uint8_t, uint8_t> * invPosition,
                          uint8_t beltPosition,
                          uint32_t count)
{
    item.setCount(count);
    ItemPosition temp =ItemPosition(invType, floorPosition, invPosition, beltPosition);
    this->mItemPositionMap[temp]=item;

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
void ItemManager::putItem(Item item,
                          Item::equipLoc to,
                          Item::equipLoc from,
                          std::pair<uint8_t, uint8_t> * invPosition,
                          uint8_t beltX,
                          std::pair<size_t, size_t> * floorPosition)
{

    std::map<ItemPosition, Item>::const_iterator it = std::find_if(
                mItemPositionMap.begin(),
                mItemPositionMap.end(),
                [&](const std::pair<ItemPosition, Item> & pair)->bool {
                    return pair.second == item;
                });

    ItemPosition pos = it->first;
    if(it== mItemPositionMap.end())
    switch(from)
    {
    case Item::eqINV:
        if(to == Item::eqFLOOR)
        {
            pos.setFloorPosition(floorPosition);
            pos.setInventoryType(Item::eqFLOOR);
        }
        break;
    case Item::eqFLOOR:
        pos.setInvPosition(invPosition);
        pos.setInventoryType(to);
        break;
    case Item::eqBELT:
        pos.setInventoryType(to);
        pos.setBeltPosition(beltX);
        break;
    case Item::eqBODY:

        pos.setInventoryType(to);
        break;
    case Item::eqRIGHTHAND:

        pos.setInventoryType(to);
        break;
    case Item::eqLEFTHAND:

        pos.setInventoryType(to);
        break;
    case Item::eqRIGHTRING:

        pos.setInventoryType(to);
        break;
    case Item::eqAMULET:

        pos.setInventoryType(to);
        break;

        break;
    case Item::eqCURSOR:

        pos.setInventoryType(to);
        break;

    case Item::eqRING:
    case Item::eqONEHAND:
    case Item::eqTWOHAND:
    default:
        return;
    }


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

Item::equipLoc ItemPosition::getInventoryType() const
{
    return this->mInventoryType;

}

void ItemPosition::setInventoryType(Item::equipLoc type)
{
    this->mInventoryType=type;
}

std::pair<size_t,size_t> * ItemPosition::getFloorPosition() const
{
    return this->mFloorPosition;

}

void ItemPosition::setFloorPosition(std::pair<size_t,size_t> * pos)
{
    this->mFloorPosition = pos;
}

std::pair<uint8_t, uint8_t> * ItemPosition::getInvPosition() const
{
    return this->mInvPosition;
}

void ItemPosition::setInvPosition(std::pair<uint8_t, uint8_t> * pos)
{
    this->mInvPosition = pos;;
}

uint8_t ItemPosition::getBeltPosition() const
{
    return this->mBeltPosition;
}

void ItemPosition::setBeltPosition(uint8_t x)
{
    this->mBeltPosition = x;
}

ItemPosition::ItemPosition(Item::equipLoc invType,
                           std::pair<size_t,size_t> * floorPosition,
                           std::pair<uint8_t, uint8_t> * invPosition,
                           uint8_t beltPosition)
{
    this->mInventoryType = invType;
    this->mFloorPosition = floorPosition;
    this->mInvPosition = invPosition;
    this->mBeltPosition = beltPosition;

}

bool ItemPosition::operator<(const ItemPosition rhs) const
{
    return mFloorPosition < rhs.mFloorPosition;

}

bool ItemPosition::operator ==(const ItemPosition rhs) const
{
    if(mInventoryType != rhs.mInventoryType)
        return false;
    if((mInvPosition != rhs.mInvPosition) || (mFloorPosition != rhs.mFloorPosition) || (mBeltPosition != rhs.mBeltPosition))
        return false;

    return true;
}


}
