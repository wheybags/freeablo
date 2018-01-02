
#pragma once

#include "itemenums.h"
#include <cel/celfile.h>
#include <diabloexe/affix.h>
#include <diabloexe/baseitem.h>
#include <diabloexe/uniqueitem.h>
#include <vector>

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace FARender
{
    class FASpriteGroup;
}

namespace FAWorld
{
    class Inventory;

    class Item
    {
    public:
        Item() = default;
        Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix* affix = nullptr, bool isIdentified = true);
        Item(const DiabloExe::UniqueItem& item, uint32_t id);

        void save(FASaveGame::GameSaver& saver);
        void load(FASaveGame::GameLoader& loader);

        std::string getFlipSoundPath() const;
        FARender::FASpriteGroup* getFlipSpriteGroup();

        bool operator==(const Item& rhs) const { return this->mUniqueId == rhs.mUniqueId; }

        ItemType getType() const { return mType; }
        ItemClass getClass() const { return mClass; }
        ItemEquipType getEquipLoc() const { return mEquipLocation; }

        uint32_t getGraphicValue() const { return mGraphicValue; }
        const std::string& getName() const { return mName; }
        const std::string& getFlipAnimationPath() const { return mDropItemGraphicsPath; }
        bool isEmpty() const { return mEmpty; }
        std::pair<uint8_t, uint8_t> getInvSize() const { return std::pair<uint8_t, uint8_t>(mSizeX, mSizeY); }
        std::pair<uint8_t, uint8_t> getInvCoords() const { return std::pair<uint8_t, uint8_t>(mInvX, mInvY); }
        std::pair<uint8_t, uint8_t> getCornerCoords() const { return std::pair<uint8_t, uint8_t>(mCornerX, mCornerY); }
        bool isBeltEquippable() const { return mSizeX == 1 && mSizeY == 1 && mUseOnce && mType != ItemType::gold; }

        // public members
        bool mIsReal = false;
        uint32_t mUniqueId = 0;
        uint32_t mCount = 0;

        static Item empty;

    private:
        static std::unique_ptr<Cel::CelFile> mObjcurs;

        std::string mName;
        ItemType mType = ItemType::misc;
        ItemClass mClass = ItemClass::none;
        ItemEquipType mEquipLocation = ItemEquipType::none;
        uint32_t mGraphicValue = 0;
        bool mUseOnce = 0;
        std::string mDropItemGraphicsPath;
        uint32_t mMaxCount = 0;
        uint8_t mBaseId = 0;
        bool mEmpty = true;

        // TODO: these should be handled by inventory class, not item class
        uint8_t mInvY = 0;
        uint8_t mInvX = 0;

        uint8_t mSizeX = 0;
        uint8_t mSizeY = 0;

        uint8_t mCornerX = 0;
        uint8_t mCornerY = 0;

        friend class Inventory;

        // variables that used to be in this class, loaded form the exe, but ommented out for now sine we don't use them.
        // Several of them also have the wrong type (eq uint8_t which should be bool)
        //
        // DiabloExe::Affix mAffix;
        //
        // uint32_t mActiveTrigger = 0;
        //
        // uint8_t mUniqCode = 0;
        // std::string mSecondName;
        // uint32_t mQualityLevel = 0;
        // uint32_t mDurability = 0;
        // uint32_t mCurrentDurability = 0;
        // bool mIsIndestructible = false;
        //
        // uint32_t mMinAttackDamage = 0;
        // uint32_t mMaxAttackDamage = 0;
        // uint32_t mAttackDamage = 0;
        //
        // uint32_t mMinArmourClass = 0;
        // uint32_t mMaxArmourClass = 0;
        // uint32_t mArmourClass = 0;
        //
        // uint8_t mReqStr = 0;
        // uint8_t mReqMagic = 0;
        // uint8_t mReqDex = 0;
        // uint8_t mReqVit = 0;
        //
        // uint32_t mSpecialEffect = 0;
        // uint32_t mMagicCode = 0;
        // uint32_t mSpellCode = 0;
        // uint32_t mBuyPrice = 0;
        // uint32_t mSellPrice = 0;
        //
        // uint8_t mNumEffects = 0;
        //
        // class ItemEffect
        // {
        // public:
        //     ItemEffectType type;
        //     int min;
        //     int max;
        // };
        //
        // // why 5?
        // std::vector<ItemEffect> mEffects = std::vector<ItemEffect>(5);
        //
        // bool mIsMagic = false;
        // bool mIsIdentified = false;
        // bool mIsUnique = false;
        //
        // uint8_t mBeltX = 0;
    };
}
