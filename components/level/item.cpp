#include "item.h"
#include "../apps/freeablo/faworld/inventory.h"
#include "itemmanager.h"
#include <render/render.h>
namespace Level

{
Item::Item()
{
    mEmpty = true;
    mCount = 0;
    mUniqueId=0;
    mBaseId = 0;
    mUniqueId=0;
    mMaxCount=0;
}
Item::~Item()
{

}

/*Item::~Item()
{
    ItemManager * itemManager = ItemManager::get();
    itemManager->removeItem(*this);

}*/

Item::Item(DiabloExe::BaseItem item, size_t id)
{
    //Render::SpriteGroup test("data/inv/objcurs.cel");
    mItem = item;
    mEmpty = false;
    mBaseId = id;
    mUniqueId = 0;

    switch(item.itemType)
    {
    case itWEAPON:
        if(item.equipLoc == eqONEHAND)
        {
            mSizeY = 3;
            mSizeX = 1;
            mMaxCount = 1;
            mCount=1;
        }
        else if(item.equipLoc == eqTWOHAND)
        {
            mSizeY = 3;
            mSizeX = 2;
            mMaxCount = 1;
            mCount=1;
        }
        break;
    case itARMOUR:
        if(
                item.equipLoc == eqONEHAND ||
                item.equipLoc == eqHEAD
                )
        {
            mSizeY = 2;
            mSizeX = 2;
            mMaxCount = 1;
            mCount=1;

        }

        else if(item.equipLoc == eqBODY)
        {
            mSizeY = 3;
            mSizeX = 2;
            mMaxCount = 1;
            mCount=1;

        }
        break;
    case itPOT:
        mSizeY = 1;
        mSizeX = 1;
        mMaxCount = 1;
        mCount=1;
        break;
    case itGOLD:
        mSizeX = 1;
        mSizeY = 1;
        mMaxCount=5000;
        break;
    case itNOVELTY:
        mSizeX = 1;
        mSizeY = 1;
        mMaxCount=1;
        mCount = 1;
        break;


    }
}
std::string Item::getName() const
{

    return this->mItem.itemName;

}

void Item::setUniqueId(uint32_t mUniqueId)
{
    this->mUniqueId = mUniqueId;
}

uint32_t Item::getUniqueId() const
{
    return this->mUniqueId;
}

uint32_t Item::getCount() const
{
    return this->mCount;
}

void Item::setCount(uint32_t count)
{
    this->mCount=count;
}

bool Item::operator ==(const Item rhs) const
{

    return this->mUniqueId == rhs.mUniqueId;
}

}
