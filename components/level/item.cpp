#include "item.h"
#include "../apps/freeablo/faworld/inventory.h"
#include "itemmanager.h"
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
Cel::CelFile * Item::mObjcurs;
bool Item::mObjcursLoaded=false;
Item::Item(DiabloExe::BaseItem item, size_t id)
{
    //Render::SpriteGroup test("data/inv/objcurs.cel");
    if(!mObjcursLoaded)
    {
        mObjcurs = new Cel::CelFile("data/inv/objcurs.cel");
        mObjcursLoaded = true;
    }
    mItem = item;
    mEmpty = false;
    mBaseId = id;
    mUniqueId = 0;

    if(mItem.itemType != itGOLD)
    {
        mItem.graphicValue +=11;
        Cel::CelFrame frame = (*mObjcurs)[mItem.graphicValue];
        mSizeX = frame.mWidth/28;
        mSizeY = frame.mHeight/28;
        mMaxCount = 1;
        mCount=1;
    }
    else
    {
        mSizeX=1;
        mSizeY=1;
        mMaxCount=5000;
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
