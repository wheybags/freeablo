#include "itemfactory.h"
#include "../falevelgen/random.h"
#include "diabloexe/diabloexe.h"
#include "item.h"
#include "itemenums.h"
#include <boost/make_unique.hpp>

namespace FAWorld
{
    ItemFactory::ItemFactory(const DiabloExe::DiabloExe& exe) : mExe(exe)
    {
        for (int i = 0; i < mExe.getBaseItems().size(); ++i)
            mUniqueBaseItemIdToItemId[mExe.getBaseItems()[i].uniqueBaseItemId] = static_cast<ItemId>(i);
    }

    Item ItemFactory::generateBaseItem(ItemId id, const BaseItemGenOptions& options) const
    {
        if (!mObjcursCel)
            mObjcursCel = boost::make_unique<Cel::CelFile>("data/inv/objcurs.cel");

        Item res;
        res.mIsIdentified = true;
        res.mEmpty = false;
        res.mUniqueId = 0;
        res.mIsReal = true;
        res.mInvX = 0;
        res.mInvY = 0;

        auto info = mExe.getBaseItems()[static_cast<int>(id)];

        res.mClass = static_cast<ItemClass>(info.itemClass);
        res.mEquipLoc = static_cast<ItemEquipType>(info.equipType);
        res.mObjCursFrame = info.invGraphicsId;

        res.mType = static_cast<ItemType>(info.type);
        res.mUniqueBaseItemId = info.uniqueBaseItemId;
        res.mName = info.name;
        res.mUnindentifiedName = info.name;
        res.mQuality = ItemQuality::normal; // ItemQuality may be changed by other functions
        res.mMaxDurability = info.durability;
        res.mCurrentDurability = info.durability;

        res.mMinAttackDamage = info.minAttackDamage;
        res.mMaxAttackDamage = info.maxAttackDamage;
        res.mArmorClass = FALevelGen::randomInRange(info.minArmorClass, info.maxArmorClass);

        res.mRequiredStrength = info.requiredStrength;
        res.mRequiredMagic = info.requiredMagic;
        res.mRequiredDexterity = info.requiredDexterity;

        res.mMiscId = static_cast<ItemMiscId>(info.miscId);
        res.mSpellId = info.spellId;
        res.mIsUsable = info.isUsable;
        res.mBuyPrice = info.price;
        res.mDropItemGraphicsPath = info.dropItemGraphicsPath;

        if (res.mClass != ItemClass::gold)
        {
            res.mObjCursFrame += 11;
            auto& frame = (*mObjcursCel)[res.mObjCursFrame];
            res.mSizeX = static_cast<uint8_t>(frame.width() / 28);
            res.mSizeY = static_cast<uint8_t>(frame.height() / 28);
            res.mMaxCount = 1;
            res.mCount = 1;
        }
        else
        {
            res.mObjCursFrame = 15;
            res.mSizeX = 1;
            res.mSizeY = 1;
            res.mMaxCount = 5000;
        }
        return res;
    }

    Item ItemFactory::generateUniqueItem(UniqueItemId id) const
    {
        auto& info = mExe.getUniqueItems()[static_cast<int32_t>(id)];
        auto it = mUniqueBaseItemIdToItemId.find(info.mUniqueBaseItemId);
        if (it == mUniqueBaseItemIdToItemId.end())
            return {};
        auto baseItemId = it->second;
        auto res = generateBaseItem(baseItemId);
        res.mBuyPrice = info.mPrice;
        res.mQuality = static_cast<ItemQuality>(info.mQualityLevel);
        res.mName = info.mName;

        for (int i = 0; i < 6; ++i)
        {
            res.mEffects[i].type = static_cast<ItemEffectType>(info.mEffectData[i][0]);
            res.mEffects[i].min = info.mEffectData[i][1];
            res.mEffects[i].max = info.mEffectData[i][2];
        }
        return res;
    }
}
