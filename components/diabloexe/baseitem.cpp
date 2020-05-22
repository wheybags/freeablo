#include "baseitem.h"
#include <faio/faio.h>
#include <iostream>
#include <sstream>

namespace DiabloExe
{

    ExeItem::ExeItem() = default;

    ExeItem::ExeItem(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        dropRate = exe.read32();

        itemClass = ItemClass(exe.read8());
        equipType = ItemEquipType(exe.read8());
        exe.read16();

        if (int32_t(equipType) == 7)
            equipType = ItemEquipType::none;

        invGraphicsId = exe.read32();

        type = ItemType(exe.read8());
        uniqueBaseItemId = exe.read8();
        exe.read16();

        uint32_t tempName = exe.read32();
        uint32_t tempSecondName = exe.read32();
        qualityLevel = exe.read32();
        durability = exe.read32();
        minAttackDamage = exe.read32();
        maxAttackDamage = exe.read32();
        minArmorClass = exe.read32();
        maxArmorClass = exe.read32();

        requiredStrength = exe.read8();
        requiredMagic = exe.read8();
        requiredDexterity = exe.read8();
        exe.read8();

        specialEffectFlags = exe.read32();
        miscId = ItemMiscId(exe.read32());
        spellId = exe.read32();
        isUsable = exe.read32();
        price = int32_t(exe.read32());
        unusedPrice = int32_t(exe.read32());

        name = exe.readCStringFromWin32Binary(tempName, codeOffset);
        if (tempSecondName)
            shortName = exe.readCStringFromWin32Binary(tempSecondName, codeOffset);
        else
            shortName = std::string();
    }

    std::string ExeItem::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
           << "\tdropRate: " << +dropRate << "," << std::endl
           << "\titemClass: " << int(itemClass) << "," << std::endl
           << "\tequipType: " << int(equipType) << "," << std::endl
           << "\tinvGraphicsId: " << +invGraphicsId << "," << std::endl
           << "\tdropItemGraphicsPath: " << dropItemGraphicsPath << std::endl
           << "\ttype: " << int(type) << "," << std::endl
           << "\tuniqCode: " << +uniqueBaseItemId << "," << std::endl

           << "\tname: " << name << "," << std::endl
           << "\titem_id: " << idName << "," << std::endl
           << "\tshortName: " << shortName << "," << std::endl
           << "\tqualityLevel: " << +qualityLevel << "," << std::endl
           << "\tdurability: " << +durability << "," << std::endl
           << "\tminAttackDamage: " << +minAttackDamage << "," << std::endl
           << "\tmaxAttackDamage: " << +maxAttackDamage << "," << std::endl

           << "\tminArmourClass: " << +minArmorClass << "," << std::endl
           << "\tmaxArmourClass: " << +maxArmorClass << "," << std::endl
           << "\treqStr: " << +requiredStrength << "," << std::endl
           << "\treqMagic: " << +requiredMagic << "," << std::endl
           << "\treqDex: " << +requiredDexterity << "," << std::endl

           << "\tspecialEffect : " << +specialEffectFlags << "," << std::endl

           << "\tmagicCode: " << int(miscId) << "," << std::endl

           << "\tspellCode: " << +spellId << "," << std::endl
           << "\tisUsable: " << +isUsable << "," << std::endl

           << "\tprice: " << +price << "," << std::endl
           << "\tunusedPrice: " << +unusedPrice << "," << std::endl
           << "}" << std::endl;
        return ss.str();
    }
}
