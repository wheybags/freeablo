#include "item.h"
#include "../falevelgen/random.h"
#include "../farender/renderer.h"
#include "../fasavegame/gameloader.h"
#include "itemenums.h"
#include "itemmanager.h"
#include <iostream>

namespace FAWorld
{
    std::unique_ptr<Cel::CelFile> Item::mObjcurs;
    Item Item::empty;

    Item::Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix* affix, bool isIdentified)
    {
        UNUSED_PARAM(affix);
        UNUSED_PARAM(isIdentified);

        if (!mObjcurs)
            mObjcurs.reset(new Cel::CelFile("data/inv/objcurs.cel"));

        mIsReal = true;
        mEmpty = false;
        mBaseId = id;
        mClass = static_cast<ItemClass>(item.itemClass);
        mEquipLocation = static_cast<ItemEquipType>(item.equipLoc);
        mGraphicValue = item.graphicValue;
        mType = static_cast<ItemType>(item.type);
        mName = item.itemName;
        mUseOnce = item.useOnce != 0;
        mDropItemGraphicsPath = item.dropItemGraphicsPath;

        if (mClass != ItemClass::gold)
        {
            mGraphicValue += 11;

            // TODO: better image loading in general, but new system should support "peeking" at image sizes, without having to decode them
            Cel::CelFrame& frame = (*mObjcurs)[mGraphicValue];
            mSizeX = static_cast<uint8_t>(frame.width() / 28);
            mSizeY = static_cast<uint8_t>(frame.height() / 28);
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

        // unused so-far attributes
        // if (affix)
        //     mAffix = *affix;
        // mIsIdentified = isIdentified;
        // mIsUnique = false;
        // mActiveTrigger = item.activTrigger;
        // mUniqCode = item.uniqCode;
        // mSecondName = item.itemSecondName;
        // mQualityLevel = item.qualityLevel;
        // mDurability = item.durability;
        // mCurrentDurability = mDurability;
        // mMinAttackDamage = item.minAttackDamage;
        // mMaxAttackDamage = item.maxAttackDamage;
        // mAttackDamage = FALevelGen::randomInRange(mMinAttackDamage, mMaxAttackDamage);
        // mMinArmourClass = item.minArmourClass;
        // mMaxArmourClass = item.maxArmourClass;
        // mArmourClass = FALevelGen::randomInRange(mMinArmourClass, mMaxArmourClass);
        // mReqStr = item.reqStr;
        // mReqMagic = item.reqMagic;
        // mReqDex = item.reqDex;
        // mReqVit = item.reqVit;
        // mSpecialEffect = item.specialEffect;
        // mMagicCode = item.magicCode;
        // mSpellCode = item.spellCode;
        // mBuyPrice = item.price1;
        // mSellPrice = item.price2;
    }

    Item::Item(const DiabloExe::UniqueItem& item, uint32_t id)
    {
        UNUSED_PARAM(item);
        UNUSED_PARAM(id);

        release_assert(false && "unique items not implemented");
        /*ItemManager& itemManager = ItemManager::get();

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
            mEffects[i].type = static_cast<ItemEffectType>(item.mEffectData[i][0]);
            mEffects[i].min = item.mEffectData[i][1];
            mEffects[i].max = item.mEffectData[i][2];
        }*/
    }

    void Item::save(FASaveGame::GameSaver& saver)
    {
        saver.save(mIsReal);
        saver.save(mUniqueId);
        saver.save(mCount);

        saver.save(mName);
        saver.save(uint8_t(mType));
        saver.save(uint8_t(mClass));
        saver.save(uint8_t(mEquipLocation));
        saver.save(mGraphicValue);
        saver.save(mUseOnce);
        saver.save(mDropItemGraphicsPath);
        saver.save(mMaxCount);
        saver.save(mBaseId);
        saver.save(mEmpty);

        saver.save(mInvY);
        saver.save(mInvX);

        saver.save(mSizeX);
        saver.save(mSizeY);

        saver.save(mCornerX);
        saver.save(mCornerY);
    }

    void Item::load(FASaveGame::GameLoader& loader)
    {
        mIsReal = loader.load<bool>();
        mUniqueId = loader.load<uint32_t>();
        mCount = loader.load<uint32_t>();

        mName = loader.load<std::string>();
        mType = ItemType(loader.load<uint8_t>());
        mClass = ItemClass(loader.load<uint8_t>());
        mEquipLocation = ItemEquipType(loader.load<uint8_t>());
        mGraphicValue = loader.load<uint32_t>();
        mUseOnce = loader.load<bool>();
        mDropItemGraphicsPath = loader.load<std::string>();
        mMaxCount = loader.load<uint32_t>();
        mBaseId = loader.load<uint8_t>();
        mEmpty = loader.load<bool>();

        mInvY = loader.load<uint8_t>();
        mInvX = loader.load<uint8_t>();

        mSizeX = loader.load<uint8_t>();
        mSizeY = loader.load<uint8_t>();

        mCornerX = loader.load<uint8_t>();
        mCornerY = loader.load<uint8_t>();
    }

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
}
