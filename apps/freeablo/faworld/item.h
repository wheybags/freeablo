#pragma once
#include "../fagui/menuentry.h"
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

namespace DiabloExe
{
    class DiabloExe;
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

    class EquipTarget;
    constexpr int indestructibleItemDurability = 255;

    class ItemEffect
    {
    public:
        ItemEffectType type;
        int min;
        int max;
    };

    class CharacterInventory;

    class Item
    {
    public:
        Item() = default;

        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);
        ~Item();
        // retrieve description which is shown when hovering over the items in your inventory
        // name including affixes/prefixes/spells
        std::string getName() const;
        ItemQuality getQuality() const;
        std::string getFullDescription() const;
        std::vector<FAGui::MenuEntry> descriptionForMerchants() const;

        void setUniqueId(uint32_t mUniqueId);
        uint32_t getUniqueId() const;
        bool operator==(const Item rhs) const;
        std::pair<uint8_t, uint8_t> getInvCoords() const;
        std::pair<uint8_t, uint8_t> getCornerCoords() const;
        bool isEmpty() const { return mEmpty; }
        std::string getFlipSoundPath() const;
        std::string getInvPlaceSoundPath() const;
        FARender::FASpriteGroup* getFlipSpriteGroup();
        bool isBeltEquippable() const;
        int32_t getMaxCount() const;
        std::array<int32_t, 2> getInvSize() const;
        int32_t getInvVolume() const;

        bool mIsReal = false;
        int32_t mCount = 1;

        int32_t getRequiredStrength() const;
        int32_t getRequiredMagic() const;
        int32_t getRequiredDexterity() const;
        uint32_t getSpecialEffect() const;
        ItemMiscId getMiscId() const;
        uint32_t getSpellCode() const;
        bool isUsable() const;
        int32_t getPrice() const;
        ItemType getType() const;
        ItemEquipType getEquipLoc() const;
        ItemClass getClass() const;
        uint32_t getGraphicValue() const;
        int32_t getMinAttackDamage() const;
        int32_t getMaxAttackDamage() const;
        ItemId baseId() const { return mBaseId; }

        // private:
        std::string chargesStr() const;
        std::string damageStr() const;
        std::string armorStr() const;
        std::string damageOrArmorStr() const;
        std::string durabilityStr() const;
        std::string requirementsStr() const;
        const DiabloExe::BaseItem& base() const;

        // private:
        static Cel::CelFile* mObjcurs;
        DiabloExe::Affix mAffix;

        ItemId mBaseId;
        int32_t mMaxDurability;
        int32_t mCurrentDurability;
        int32_t mArmorClass;

        // TODO: these should be handled by inventory class, not item class
        int32_t mCornerX = -1;
        int32_t mCornerY = -1;

        bool mEmpty = true;
        bool mIsIdentified;

        int32_t mUniqueId = 0;

        // private:
        int32_t mMaxCharges = 0;
        int32_t mCurrentCharges = 0;

        // TODO: these should be handled by inventory class, not item class
        uint8_t mInvY = 0;
        uint8_t mInvX = 0;

        static bool isItemAMeleeWeapon(ItemType type);
        static bool isItemARangedWeapon(ItemType type);
        static bool isItemAWeapon(ItemType type) { return isItemAMeleeWeapon(type) || isItemARangedWeapon(type); }

        friend class CharacterInventory;
        friend class ItemFactory;
        friend class PlayerFactory;
    };
}
