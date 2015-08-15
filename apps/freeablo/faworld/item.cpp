#include "item.h"
#include "itemmanager.h"
namespace FAWorld
{
Item::Item()
{
    mEmpty = true;
    mCount = 0;
    mUniqueId=0;
    mBaseId = 0;
    mUniqueId=0;
    mMaxCount=0;
    mInvX=0;
    mInvY=0;
    mSizeX=0;
    mSizeY=0;
    mCornerX=0;
    mCornerY=0;
    mIsReal=false;
}
Item::~Item()
{



}


Cel::CelFile * Item::mObjcurs;
bool Item::mObjcursLoaded=false;
Item::Item(DiabloExe::BaseItem item, size_t id)
{

    if(!mObjcursLoaded)
    {
        mObjcurs = new Cel::CelFile("data/inv/objcurs.cel");
        mObjcursLoaded = true;
    }
    mItem = item;
    mEmpty = false;
    mBaseId = id;
    mUniqueId = 0;
    mIsReal = true;
    mInvX=0;
    mInvY=0;

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
bool Item::isReal() const
{
    return mIsReal;
}
std::string Item::getName() const
{

    return this->mItem.itemName;

}
std::pair<uint8_t, uint8_t> Item::getInvSize() const
{
    return std::pair<uint8_t, uint8_t>(mSizeX, mSizeY);

}
std::pair<uint8_t, uint8_t> Item::getInvCoords() const
{
    return std::pair<uint8_t, uint8_t>(mInvX, mInvY);

}
std::pair<uint8_t, uint8_t> Item::getCornerCoords() const
{
    return std::pair<uint8_t, uint8_t>(mCornerX, mCornerY);
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
uint8_t Item::getBeltX() const
{
    return mBeltX;

}

}
