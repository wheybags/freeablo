#include "item.h"
#include "../falevelgen/random.h"
#include "../farender/renderer.h"
#include "../fasavegame/gameloader.h"
#include "itemenums.h"
#include <boost/format.hpp>
#include <iostream>

namespace FAWorld
{
    std::unique_ptr<Cel::CelFile> Item::mObjcurs;
    Item Item::empty;

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

    std::string Item::getFullDescription() const
    {
        if (mType == ItemType::gold)
            return (boost::format("%1% gold %2%") % mCount % (mCount ? "pieces" : "piece")).str();

        auto description = getFullName();
        if (mClass == ItemClass::weapon)
            description += (boost::format("damage: %1% - %2%") % mMinAttackDamage % mMaxAttackDamage).str();
        if (mClass == ItemClass::armor)
            description += (boost::format("armor: %1%") % mArmorClass).str();

        if (mClass == ItemClass::armor || mClass == ItemClass::weapon)
        {
            description += " ";
            if (mMaxDurability == indestructibleItemDurability)
                description += "Indestructible";
            else
                description += (boost::format("Dur: %1%/%2%") % mCurrentDurability % mMaxDurability).str();
            description += "\n";
        }

        if (mMiscId == ItemMiscId::staff && mMaxCharges > 0)
            {
                description += (boost::format("Charges: %1%/%2%\n") % mCurrentCharges % mMaxCharges).str();
            }
        if (mQuality == ItemQuality::unique)
            description += "unique item\n";
        // TODO: affix/prefix description
        return description;
    }

    std::pair<uint8_t, uint8_t> Item::getInvSize() const { return {mSizeX, mSizeY}; }
    std::pair<uint8_t, uint8_t> Item::getInvCoords() const { return {mInvX, mInvY}; }
    std::pair<uint8_t, uint8_t> Item::getCornerCoords() const { return {mCornerX, mCornerY}; }

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

    bool Item::isBeltEquippable() const { return mSizeX == 1 && mSizeY == 1 && mIsUsable && mType != ItemType::gold; }

    uint32_t Item::getActiveTrigger() const { return isUsable; }
    uint8_t Item::getReqStr() const { return mRequiredStrength; }
    uint8_t Item::getReqMagic() const { return mRequiredMagic; }

    uint8_t Item::getReqDex() const { return mRequiredDexterity; }

    uint32_t Item::getSpecialEffect() const { return mSpecialEffectFlags; }

    ItemMiscId Item::getMiscId() const { return mMiscId; }

    uint32_t Item::getSpellCode() const { return mSpellId; }

    uint32_t Item::getUseOnce() const { return mIsUsable; }

    uint32_t Item::getBuyPrice() const { return mBuyPrice; }

    ItemType Item::getType() const { return mType; }

    ItemEquipType Item::getEquipLoc() const { return mEquipLoc; }

    ItemClass Item::getClass() const { return mClass; }
    uint32_t Item::getGraphicValue() const { return mObjCursFrame; }

    void Item::setUniqueId(uint32_t mUniqueId) { this->mUniqueId = mUniqueId; }

    uint32_t Item::getUniqueId() const { return this->mUniqueId; }

    uint32_t Item::getCount() const { return this->mCount; }

    void Item::setCount(uint32_t count)
    {
        if (count <= mMaxCount)
        {
            if (count <= 1000)
                mObjCursFrame = 15;
            else if (count > 1000 && count < 2500)
                mObjCursFrame = 16;
            else
                mObjCursFrame = 17;
            this->mCount = count;
        }
    }

    bool Item::operator==(const Item rhs) const { return this->mUniqueId == rhs.mUniqueId; }
    uint8_t Item::getBeltX() const { return mBeltX; }
}
