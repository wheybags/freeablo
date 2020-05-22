#pragma once
#include "../fagui/menuentry.h"
#include "itemenums.h"
#include <cel/celfile.h>
#include <diabloexe/affix.h>
#include <diabloexe/baseitem.h>
#include <diabloexe/uniqueitem.h>
#include <vector>

namespace Render
{
    class SpriteGroup;
}

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace DiabloExe
{
    class DiabloExe;
}

namespace FAWorld
{
    constexpr int32_t indestructibleItemDurability = 255;

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

        bool operator==(const Item&) = delete;

        std::pair<uint8_t, uint8_t> getCornerCoords() const;
        bool isEmpty() const { return mEmpty; }
        std::string getFlipSoundPath() const;
        std::string getInvPlaceSoundPath() const;
        Render::SpriteGroup* getFlipSpriteGroup() const;
        bool isBeltEquippable() const;
        int32_t getMaxCount() const;
        std::array<int32_t, 2> getInvSize() const;
        int32_t getInvVolume() const;

        bool mIsReal = false;
        int32_t mCount = 1;

        int32_t getRequiredStrength() const;
        int32_t getRequiredMagic() const;
        int32_t getRequiredDexterity() const;
        ItemMiscId getMiscId() const;
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
        const DiabloExe::ExeItem& base() const;

        // private:
        ItemId mBaseId = ItemId::COUNT;
        int32_t mMaxDurability = 0;
        int32_t mCurrentDurability = 0;
        int32_t mArmorClass = 0;

        // TODO: these should be handled by inventory class, not item class
        int32_t mCornerX = -1;
        int32_t mCornerY = -1;

        bool mEmpty = true;

        // private:
        int32_t mMaxCharges = 0;
        int32_t mCurrentCharges = 0;

        // TODO: these should be handled by inventory class, not item class
        uint8_t mInvY = 0;
        uint8_t mInvX = 0;

        static bool isItemAMeleeWeapon(ItemType type);
        static bool isItemARangedWeapon(ItemType type);
        static bool isItemAWeapon(ItemType type) { return isItemAMeleeWeapon(type) || isItemARangedWeapon(type); }
    };
}
