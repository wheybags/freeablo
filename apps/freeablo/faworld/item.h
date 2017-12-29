
#pragma once

#include <array>
#include <cel/celfile.h>
#include <diabloexe/affix.h>
#include <diabloexe/baseitem.h>
#include <diabloexe/uniqueitem.h>

namespace FARender
{
    class FASpriteGroup;
    class ItemType;
}

namespace FAWorld
{
    enum class ItemEffectType;
    enum class ItemType;
    enum class ItemEquipType;
    enum class ItemClass;

    class ItemEffect
    {
    public:
        ItemEffectType type;
        int min;
        int max;
    };

    class Inventory;
    class ItemPosition;
    class Item
    {
        friend class FAWorld::Inventory;
        friend class ItemPosition;

    public:
        bool isEmpty() const;
        Item();
        ~Item();
        Item(DiabloExe::BaseItem item, uint32_t id, DiabloExe::Affix* affix = nullptr, bool isIdentified = true);
        Item(const DiabloExe::UniqueItem& item, uint32_t id);
        std::string getName() const;
        void setUniqueId(uint32_t mUniqueId);
        uint32_t getUniqueId() const;
        void setCount(uint32_t count);
        uint32_t getCount() const;
        uint8_t getBeltX() const;
        bool operator==(const Item rhs) const;
        bool isReal() const;
        std::pair<uint8_t, uint8_t> getInvSize() const;
        std::pair<uint8_t, uint8_t> getInvCoords() const;
        std::pair<uint8_t, uint8_t> getCornerCoords() const;
        std::string getFlipAnimationPath() const;
        std::string getFlipSoundPath() const;
        FARender::FASpriteGroup* getFlipSpriteGroup();
        bool isBeltEquippable() const;

        bool mIsReal;

        uint32_t getActiveTrigger() const;
        uint8_t getReqStr() const;
        uint8_t getReqMagic() const;
        uint8_t getReqDex() const;
        uint8_t getReqVit() const;
        uint32_t getSpecialEffect() const;
        uint32_t getMagicCode() const;
        uint32_t getSpellCode() const;
        uint32_t getUseOnce() const;
        uint32_t getBuyPrice() const;
        uint32_t getSellPrice() const;
        ItemType getType() const;
        ItemEquipType getEquipLoc() const;
        ItemClass getClass() const;
        uint32_t getGraphicValue() const;
        static Item empty;

    private:
        static Cel::CelFile* mObjcurs;
        DiabloExe::Affix mAffix;

        uint32_t mActiveTrigger;
        ItemClass mClass;
        ItemEquipType mEquipLoc;
        uint32_t mGraphicValue;

        ItemType mType;
        uint8_t mUniqCode;
        std::string mName;
        std::string mSecondName;
        uint32_t mQualityLevel;
        uint32_t mDurability;
        uint32_t mCurrentDurability;
        bool mIsIndestructible;

        uint32_t mMinAttackDamage;
        uint32_t mMaxAttackDamage;
        uint32_t mAttackDamage;

        uint32_t mMinArmourClass;
        uint32_t mMaxArmourClass;
        uint32_t mArmourClass;

        uint8_t mReqStr;
        uint8_t mReqMagic;
        uint8_t mReqDex;
        uint8_t mReqVit;

        uint32_t mSpecialEffect;
        uint32_t mMagicCode;
        uint32_t mSpellCode;
        uint32_t mUseOnce;
        uint32_t mBuyPrice;
        uint32_t mSellPrice;

        uint8_t mNumEffects;

        std::array<ItemEffect, 5> mEffects;

        uint8_t mBaseId;
        uint32_t mUniqueId;

        uint32_t mCount;
        uint32_t mMaxCount;

        uint8_t mCornerX;
        uint8_t mCornerY;

        bool mEmpty;
        bool mIsMagic;
        bool mIsIdentified;
        bool mIsUnique;

        uint8_t mInvY;
        uint8_t mInvX;

        uint8_t mBeltX;

        uint8_t mSizeX;
        uint8_t mSizeY;

        static bool mObjcursLoaded;
        std::string mDropItemGraphicsPath;
    };
}
