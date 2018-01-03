
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
    enum class ItemEffectType;
    enum class ItemType;
    enum class ItemEquipType;
    enum class ItemClass;
    enum class ItemCode;
    enum class ItemMiscId;
    enum class ItemQuality;
    constexpr int indestructibleItemDurability = 255;

    class ItemEffect
    {
    public:
        ItemEffectType type;
        int min;
        int max;
    };

    class Inventory;

    class Item
    {
    public:
        Item() = default;

        void save(FASaveGame::GameSaver& saver);
        void load(FASaveGame::GameLoader& loader);
        bool isEmpty() const;
        Item();
        ~Item();
        Item(DiabloExe::BaseItem item, DiabloExe::Affix* affix = nullptr, bool isIdentified = true);
        Item(const DiabloExe::UniqueItem& item, uint32_t id);
        std::string getName() const;
        // name including affixes/prefixes/spells
        std::string getFullName() const;
        // retrieve description which is shown when hovering over the items in your inventory
        std::string getFullDescription () const;

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
        ItemMiscId getMiscId() const;
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

        uint32_t isUsable;
        ItemClass mClass;
        ItemEquipType mEquipLoc;
        uint32_t mObjCursFrame;
        int32_t mCurrentCharges;
        int32_t mMaxCharges;

        ItemType mType;
        uint8_t mUniqueBaseItemId;
        std::string mName;
        std::string mUnindentifiedName;
        ItemQuality mQuality;
        uint32_t mMaxDurability;
        uint32_t mCurrentDurability;

        uint32_t mMinAttackDamage;
        uint32_t mMaxAttackDamage;
        uint32_t mAttackDamage;

        uint32_t mArmorClass;

        int32_t mRequiredStrength;
        int32_t mRequiredMagic;
        int32_t mRequiredDexterity;

        uint32_t mSpecialEffectFlags;
        ItemCode mCode;
        uint32_t mSpellId;
        uint32_t mIsUsable;
        uint32_t mBuyPrice;

        std::string getFlipSoundPath() const;
        FARender::FASpriteGroup* getFlipSpriteGroup();

        bool operator==(const Item& rhs) const { return this->mUniqueId == rhs.mUniqueId; }

        uint8_t mCornerX;
        uint8_t mCornerY;
        ItemMiscId mMiscId;

        bool mEmpty;
        bool mIsIdentified;

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
    };
}

