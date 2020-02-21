#pragma once
#include <faio/fafileobject.h>
#include <map>
#include <stdint.h>
#include <string>

namespace DiabloExe
{
    class BaseItem
    {
    public:
        uint32_t dropRate;

        uint8_t itemClass;
        uint8_t equipType;

        uint16_t unknown0;

        uint32_t invGraphicsId;

        uint8_t type;
        uint8_t uniqueBaseItemId;
        uint16_t unknown1;

        std::string name;
        std::string shortName;
        uint32_t qualityLevel;
        uint32_t durability;
        uint32_t minAttackDamage;
        uint32_t maxAttackDamage;
        uint32_t minArmorClass;
        uint32_t maxArmorClass;

        uint8_t requiredStrength;
        uint8_t requiredMagic;
        uint8_t requiredDexterity;
        uint8_t unused;

        uint32_t specialEffectFlags;
        uint32_t miscId;
        uint32_t spellId;
        uint32_t isUsable;
        uint32_t price;
        uint32_t unusedPrice;
        int32_t id;
        int32_t invSizeX;
        int32_t invSizeY;

        std::string dropItemGraphicsPath;
        std::string dropItemSoundPath;
        std::string invPlaceItemSoundPath;

        std::string dump() const;
        BaseItem();

    private:
        BaseItem(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;
    };
}
