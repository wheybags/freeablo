#include "item.h"
#include "../falevelgen/random.h"
#include "../farender/renderer.h"
#include "itemmanager.h"
#include <iostream>

namespace FAWorld
{
    Item Item::empty;
    Item::Item()
    {
        mEmpty = true;
        mCount = 0;
        mUniqueId = 0;
        mBaseId = 0;
        mUniqueId = 0;
        mMaxCount = 0;
        mInvX = 0;
        mInvY = 0;
        mSizeX = 0;
        mSizeY = 0;
        mCornerX = 0;
        mCornerY = 0;
        mIsReal = false;
    }
    Item::~Item() {}

    Cel::CelFile* Item::mObjcurs;
    bool Item::mObjcursLoaded = false;
    Item::Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix* affix, bool isIdentified)
    {

        if (!mObjcursLoaded)
        {
            mObjcurs = new Cel::CelFile("data/inv/objcurs.cel");
            mObjcursLoaded = true;
        }

        mIsUnique = false;
        mEmpty = false;
        mBaseId = id;
        mUniqueId = 0;
        mIsReal = true;
        mInvX = 0;
        mInvY = 0;

        mActiveTrigger = item.activTrigger;

        mClass = static_cast<ItemClass>(item.itemClass);
        mEquipLoc = static_cast<ItemEquipType>(item.equipLoc);
        mGraphicValue = item.graphicValue;

        mType = static_cast<ItemType>(item.type);
        mUniqCode = item.uniqCode;
        mName = item.itemName;
        mSecondName = item.itemSecondName;
        mQualityLevel = item.qualityLevel;
        mDurability = item.durability;
        mCurrentDurability = mDurability;

        mMinAttackDamage = item.minAttackDamage;
        mMaxAttackDamage = item.maxAttackDamage;
        mAttackDamage = FALevelGen::randomInRange(mMinAttackDamage, mMaxAttackDamage);
        mMinArmourClass = item.minArmourClass;
        mMaxArmourClass = item.maxArmourClass;
        mArmourClass = FALevelGen::randomInRange(mMinArmourClass, mMaxArmourClass);
        mReqStr = item.reqStr;
        mReqMagic = item.reqMagic;
        mReqDex = item.reqDex;
        mReqVit = item.reqVit;

        mSpecialEffect = item.specialEffect;
        mMagicCode = item.magicCode;
        mSpellCode = item.spellCode;
        mUseOnce = item.useOnce;
        mBuyPrice = item.price1;
        mSellPrice = item.price2;
        mDropItemGraphicsPath = item.dropItemGraphicsPath;

        if (mClass != ItemClass::gold)
        {
            mGraphicValue += 11;
            Cel::CelFrame frame = (*mObjcurs)[mGraphicValue];
            mSizeX = static_cast<uint8_t>(frame.mWidth / 28);
            mSizeY = static_cast<uint8_t>(frame.mHeight / 28);
            mMaxCount = 1;
            mCount = 1;
        }
        else
        {
            mGraphicValue = 15;
            mSizeX = 1;
            mSizeY = 1;
            mMaxCount = 5000;
        }
    }
    Item::Item(const DiabloExe::UniqueItem& item, uint32_t id)
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

        for (int i = 0; i < 6; ++i)
            {
                mEffects[i].type = static_cast<ItemEffectType> (item.mEffectData[i][0]);
                mEffects[i].min = item.mEffectData[i][1];
                mEffects[i].max = item.mEffectData[i][2];
            }
    }

    bool Item::isEmpty() const { return mEmpty; }

    bool Item::isReal() const { return mIsReal; }
    std::string Item::getName() const { return this->mName; }
    std::pair<uint8_t, uint8_t> Item::getInvSize() const { return std::pair<uint8_t, uint8_t>(mSizeX, mSizeY); }
    std::pair<uint8_t, uint8_t> Item::getInvCoords() const { return std::pair<uint8_t, uint8_t>(mInvX, mInvY); }
    std::pair<uint8_t, uint8_t> Item::getCornerCoords() const { return std::pair<uint8_t, uint8_t>(mCornerX, mCornerY); }

    std::string Item::getFlipAnimationPath() const { return mDropItemGraphicsPath; }

    std::string Item::getFlipSoundPath() const
    {
        // TODO: add book, pot, scroll
        switch (getType())
        {
            case ItemType::misc:
                return "";
            case ItemType::sword:
                return "sfx/items/flipswor.wav";
            case ItemType::axe:
                return "sfx/items/flipaxe.wav";
            case ItemType::bow:
                return "sfx/items/flipbow.wav";
            case ItemType::mace:
                return "sfx/items/flipswor.wav"; // TODO: check
            case ItemType::shield:
                return "sfx/items/flipshld.wav";
            case ItemType::lightArmor:
                return "sfx/items/fliplarm.wav";
            case ItemType::helm:
                return "sfx/items/flipcap.wav";
            case ItemType::mediumArmor:
                return "sfx/items/fliplarm.wav"; // TODO: check
            case ItemType::heavyArmor:
                return "sfx/items/flipharm.wav";
            case ItemType::staff:
                return "sfx/items/flipstaf.wav";
            case ItemType::gold:
                return "sfx/items/gold.wav"; // also gold1.cel
            case ItemType::ring:
            case ItemType::amulet:
                return "sfx/items/flipring.wav";
        }
        return "";
    }

    FARender::FASpriteGroup* Item::getFlipSpriteGroup() { return FARender::Renderer::get()->loadImage(getFlipAnimationPath()); }

    bool Item::isBeltEquippable() const { return mSizeX == 1 && mSizeY == 1 && mUseOnce && mType != ItemType::gold; }

    uint32_t Item::getActiveTrigger() const { return mActiveTrigger; }
    uint8_t Item::getReqStr() const { return mReqStr; }
    uint8_t Item::getReqMagic() const { return mReqMagic; }

    uint8_t Item::getReqDex() const { return mReqDex; }

    uint8_t Item::getReqVit() const { return mReqVit; }

    uint32_t Item::getSpecialEffect() const { return mSpecialEffect; }

    uint32_t Item::getMagicCode() const { return mMagicCode; }

    uint32_t Item::getSpellCode() const { return mSpellCode; }

    uint32_t Item::getUseOnce() const { return mUseOnce; }

    uint32_t Item::getBuyPrice() const { return mBuyPrice; }
    uint32_t Item::getSellPrice() const { return mSellPrice; }

    ItemType Item::getType() const { return mType; }

    ItemEquipType Item::getEquipLoc() const { return mEquipLoc; }

    ItemClass Item::getClass() const { return mClass; }
    uint32_t Item::getGraphicValue() const { return mGraphicValue; }

    void Item::setUniqueId(uint32_t mUniqueId) { this->mUniqueId = mUniqueId; }

    uint32_t Item::getUniqueId() const { return this->mUniqueId; }

    uint32_t Item::getCount() const { return this->mCount; }

    void Item::setCount(uint32_t count)
    {
        if (count <= mMaxCount)
        {
            if (count <= 1000)
                mGraphicValue = 15;
            else if (count > 1000 && count < 2500)
                mGraphicValue = 16;
            else
                mGraphicValue = 17;
            this->mCount = count;
        }
    }

    bool Item::operator==(const Item rhs) const { return this->mUniqueId == rhs.mUniqueId; }
    uint8_t Item::getBeltX() const { return mBeltX; }
}
