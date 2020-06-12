#pragma once
#include <cstdint>
#include <faio/fafileobject.h>
#include <map>
#include <misc/commonenums.h>
#include <string>

enum class ItemMiscId
{
    none = 0,
    potionOfFullHealing = 2,
    potionOfHealing = 3,
    potionOfMana = 6,
    potionOfFullMana = 7,
    elixirOfStrength = 10,
    elixirOfMagic = 11,
    elixirOfDexterity = 12,
    elixirOfVitality = 13,
    potionOfRejuvenation = 18,
    potionOfFullRejuvenation = 19,
    scroll = 21,
    scrollWithTarget = 22,
    staff = 23,
    book = 24,
    ring = 25,
    amulet = 26,
    unique = 27,
    mapOfTheStars = 42,
    ear = 43,
    spectralElixir = 44,
    invalid = -1,
};

namespace DiabloExe
{
    class ExeItem
    {
    public:
        uint32_t dropRate = 0;

        ItemClass itemClass = ItemClass::none;
        ItemEquipType equipType = ItemEquipType::none;

        uint32_t invGraphicsId = 0;

        ItemType type = ItemType::none;
        uint8_t uniqueBaseItemId = 0;

        std::string name;
        std::string idName;
        std::string shortName;
        uint32_t qualityLevel = 0;
        uint32_t durability = 0;
        uint32_t minAttackDamage = 0;
        uint32_t maxAttackDamage = 0;
        uint32_t minArmorClass = 0;
        uint32_t maxArmorClass = 0;

        uint8_t requiredStrength = 0;
        uint8_t requiredMagic = 0;
        uint8_t requiredDexterity = 0;

        uint32_t specialEffectFlags = 0;
        ItemMiscId miscId = ItemMiscId::none;
        uint32_t spellId = 0;
        uint32_t isUsable = 0;
        int32_t price = 0;
        int32_t unusedPrice = 0;
        int32_t invSizeX = 0;
        int32_t invSizeY = 0;

        std::string dropItemGraphicsPath;
        std::string dropItemSoundPath;
        std::string invPlaceItemSoundPath;

        std::string dump() const;
        ExeItem();

    private:
        ExeItem(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;
    };
}
