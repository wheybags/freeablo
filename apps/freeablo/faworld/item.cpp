#include "item.h"
#include "itemmanager.h"
#include <iostream>
#include "../falevelgen/random.h"
#include "../farender/renderer.h"

namespace FAWorld
{
Item Item::empty;
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
Item::Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix* affix, bool isIdentified)
{

    if(!mObjcursLoaded)
    {
        mObjcurs = new Cel::CelFile("data/inv/objcurs.cel");
        mObjcursLoaded = true;
    }

    mIsUnique = false;
    mEmpty = false;
    mBaseId = id;
    mUniqueId = 0;
    mIsReal = true;
    mInvX=0;
    mInvY=0;

    mActiveTrigger = item.activTrigger;

    mType = static_cast<itemType>(item.itemType);
    mEquipLoc = static_cast<equipLoc>(item.equipLoc);
    mGraphicValue = item.graphicValue;

    mCode = static_cast<itemCode>(item.itemCode);
    mUniqCode = item.uniqCode;
    mName = item.itemName;
    mSecondName = item.itemSecondName;
    mQualityLevel = item.qualityLevel;
    mDurability = item.durability;
    mCurrentDurability = mDurability;

    mMinAttackDamage = item.minAttackDamage;
    mMaxAttackDamage = item.maxAttackDamage;
    mAttackDamage = FALevelGen::randomInRange(mMinAttackDamage,
                                              mMaxAttackDamage);
    mMinArmourClass = item.minArmourClass;
    mMaxArmourClass = item.maxArmourClass;
    mArmourClass = FALevelGen::randomInRange(mMinArmourClass,
                                             mMaxArmourClass);
    mReqStr = item.reqStr;
    mReqMagic = item.reqMagic;
    mReqDex = item.reqDex;
    mReqVit = item.reqVit;

    mSpecialEffect = item.specialEffect;
    mMagicCode = item.magicCode;
    mSpellCode = item.spellCode;
    mUseOnce  = item.useOnce;
    mBuyPrice = item.price1;
    mSellPrice= item.price2;

    if(mType != itGOLD)
    {

        mGraphicValue +=11;
        Cel::CelFrame frame = (*mObjcurs)[mGraphicValue];
        mSizeX = static_cast<uint8_t> (frame.mWidth/28);
        mSizeY = static_cast<uint8_t> (frame.mHeight/28);
        mMaxCount = 1;
        mCount=1;
    }
    else
    {
        mGraphicValue=15;
        mSizeX=1;
        mSizeY=1;
        mMaxCount=5000;
    }

    if(affix != NULL)
    {
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect0), mMaxRange0, mMinRange0, id));
        mIsMagic   = true;
        mEffect0   = affix->mEffect;
        mMaxRange0 = affix->mMaxEffect;
        mMinRange0 = affix->mMinEffect;
        mIsIdentified = isIdentified;



    }

}
Item::Item(const DiabloExe::UniqueItem & item, uint32_t id)
{
    ItemManager& itemManager = ItemManager::get();

    DiabloExe::BaseItem& baseItem = itemManager.getBaseItemByUniqueCode(item.mItemType);

    Item base = Item(baseItem, id);
    *this = base;
    mIsUnique = true;
    mIsMagic = false;
    mSellPrice = item.mGoldValue;
    mBuyPrice = item.mGoldValue;
    mQualityLevel = item.mQualityLevel;
    mName = item.mName;

    mEffect0 = item.mEffect0;
    mMinRange0 = item.mMinRange0;
    mMaxRange0 = item.mMaxRange0;

    mEffect1 = item.mEffect1;
    mMinRange1 = item.mMinRange1;
    mMaxRange1 = item.mMaxRange1;

    mEffect2 = item.mEffect2;
    mMinRange2 = item.mMinRange2;
    mMaxRange2 = item.mMaxRange2;

    mEffect3 = item.mEffect3;
    mMinRange3 = item.mMinRange3;
    mMaxRange3 = item.mMaxRange3;

    mEffect4 = item.mEffect4;
    mMinRange4 = item.mMinRange4;
    mMaxRange4 = item.mMaxRange4;

    mEffect5 = item.mEffect5;
    mMinRange5 = item.mMinRange5;
    mMaxRange5 = item.mMaxRange5;

    if(mMaxRange0 != 0 || mEffect0 != 0 || mMinRange0 != 0)
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect0), mMaxRange0, mMinRange0, id));
    if(mMaxRange1 != 0 || mEffect1 != 0 || mMinRange1 != 0)
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect1), mMaxRange1, mMinRange1, id));
    if(mMaxRange2 != 0 || mEffect2 != 0 || mMinRange2 != 0)
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect2), mMaxRange2, mMinRange2, id));
    if(mMaxRange3 != 0 || mEffect3 != 0 || mMinRange3 != 0)
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect3), mMaxRange3, mMinRange3, id));
    if(mMaxRange4 != 0 || mEffect4 != 0 || mMinRange4 != 0)
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect4), mMaxRange4, mMinRange4, id));
    if(mMaxRange5 != 0 || mEffect5 != 0 || mMinRange5 != 0)
        mEffects.push_back(std::tuple<ItemEffect, uint32_t, uint32_t, uint32_t>(static_cast<ItemEffect>(mEffect5), mMaxRange5, mMinRange5, id));


}

bool Item::isEmpty() const
{
    return mEmpty;
}

bool Item::isReal() const
{
    return mIsReal;
}
std::string Item::getName() const
{

    return this->mName;

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

std::string Item::getFlipAnimationPath() const
{
    // TODO: extract mapping from exe
    switch (getCode())
    {
        case icOther:
            switch (getType())
            {
                case itPOT: return "items/fbttle.cel";
            default:
                break;
            }
            break;
        case icSword: return "items/swrdflip.cel";
        case icAxe: return "items/axe.cel";
        case icBow: return "items/bow.cel";
        case icBlunt: return "items/mace.cel";
        case icShield: return "items/shield.cel";
        case icLightArmour: return "items/larmor.cel";
        case icHelm: return "items/helmut.cel";
        case icMidArmour: return "items/armor2.cel";
        case icHeavyArmour: return "items/FPlateAr.cel";
        case icStave: return "items/staff.cel";
        case icGold: return "items/goldflip.cel";
        case icRing:
        case icAmulet:
            return "items/ring.cel";
    }
    return "";
}

std::string Item::getFlipSoundPath() const
{
    // TODO: add book, pot, scroll
    switch (getCode())
    {
        case icOther:
            return "";
        case icSword: return "sfx/items/flipswor.wav";
        case icAxe: return "sfx/items/flipaxe.wav";
        case icBow: return "sfx/items/flipbow.wav";
        case icBlunt: return "sfx/items/flipswor.wav"; // TODO: check
        case icShield: return "sfx/items/flipshld.wav";
        case icLightArmour: return "sfx/items/fliplarm.wav";
        case icHelm: return "sfx/items/flipcap.wav";
        case icMidArmour: return "sfx/items/fliplarm.wav"; // TODO: check
        case icHeavyArmour: return "sfx/items/flipharm.wav";
        case icStave: return "sfx/items/flipstaf.wav";
        case icGold: return "sfx/items/gold.wav"; // also gold1.cel
        case icRing:
        case icAmulet:
            return "sfx/items/flipring.wav";
    }
    return "";
}

FARender::FASpriteGroup* Item::getFlipSpriteGroup()
{
    return FARender::Renderer::get()->loadImage (getFlipAnimationPath());
}

bool Item::isBeltEquippable() const
{
    return mSizeX == 1 && mSizeY == 1 && mUseOnce && mCode != icGold;
}

    uint32_t Item::getActiveTrigger() const
{
    return mActiveTrigger;
}
uint8_t Item::getReqStr() const
{
    return mReqStr;
}
uint8_t Item::getReqMagic() const
{
    return mReqMagic;
}

uint8_t Item::getReqDex() const
{
    return mReqDex;
}

uint8_t Item::getReqVit() const
{
    return mReqVit;
}

uint32_t Item::getSpecialEffect() const
{
    return mSpecialEffect;
}

uint32_t Item::getMagicCode() const
{
    return mMagicCode;
}

uint32_t Item::getSpellCode() const
{
    return mSpellCode;
}

uint32_t Item::getUseOnce() const
{
    return mUseOnce;
}

uint32_t Item::getBuyPrice() const
{
    return mBuyPrice;
}
uint32_t Item::getSellPrice() const
{
    return mSellPrice;
}

uint32_t Item::getEffect0() const
{
    return mEffect0;
}

uint32_t Item::getMinRange0() const
{
    return mMinRange0;
}

uint32_t Item::getMaxRange0() const
{
    return mMaxRange0;
}

uint32_t Item::getEffect1() const
{
    return mEffect1;
}

uint32_t Item::getMinRange1() const
{
    return mMinRange1;
}

uint32_t Item::getMaxRange1() const
{
    return mMaxRange1;
}

uint32_t Item::getEffect2() const
{
    return mEffect2;
}

uint32_t Item::getMinRange2() const
{
    return mMinRange2;
}

uint32_t Item::getMaxRange2() const
{
    return mMaxRange2;
}

uint32_t Item::getEffect3() const
{
    return mEffect3;
}

uint32_t Item::getMinRange3() const
{
    return mMinRange3;
}

uint32_t Item::getMaxRange3() const
{
    return mMaxRange3;
}

uint32_t Item::getEffect4() const
{
    return mEffect4;
}

uint32_t Item::getMinRange4() const
{
    return mMinRange4;
}

uint32_t Item::getMaxRange4() const
{
    return mMaxRange4;
}

uint32_t Item::getEffect5() const
{
    return mEffect5;
}

uint32_t Item::getMinRange5() const
{
    return mMinRange5;
}

uint32_t Item::getMaxRange5() const
{
    return mMaxRange5;
}

Item::itemCode Item::getCode() const
{
    return mCode;
}

Item::equipLoc Item::getEquipLoc() const
{
    return mEquipLoc;
}

Item::itemType Item::getType() const
{
    return mType;
}
uint32_t Item::getGraphicValue() const
{
    return mGraphicValue;
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
    if(count <= mMaxCount)
    {
        if(count <= 1000)
            mGraphicValue=15;
        else if(count > 1000 && count < 2500)
            mGraphicValue=16;
        else
            mGraphicValue=17;
        this->mCount=count;
    }
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
