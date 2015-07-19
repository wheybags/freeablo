#include "baseitem.h"
#include <faio/faio.h>

#include <iostream>
#include <sstream>

namespace DiabloExe
{

    BaseItem::BaseItem(FAIO::FAFile *exe, size_t codeOffset)
    {
        activTrigger = FAIO::read32(exe);

        itemType = FAIO::read8(exe);
        equipLoc = FAIO::read8(exe);

        unknown0 = FAIO::read16(exe);

        graphicValue = FAIO::read32(exe);

        itemCode = FAIO::read8(exe);
        uniqCode = FAIO::read8(exe);

        unknown1 = FAIO::read16(exe);


        uint32_t tempName = FAIO::read32(exe);
        uint32_t tempSecondName = FAIO::read32(exe);
        qualityLevel = FAIO::read32(exe);
        durability = FAIO::read32(exe);
        minAttackDamage = FAIO::read32(exe);
        maxAttackDamage = FAIO::read32(exe);
        minArmourClass  = FAIO::read32(exe);
        maxArmourClass  = FAIO::read32(exe);

        reqStr = FAIO::read8(exe);
        reqMagic = FAIO::read8(exe);
        reqDex   = FAIO::read8(exe);
        reqVit   = FAIO::read8(exe);

        specialEffect = FAIO::read32(exe);
        magicCode = FAIO::read32(exe);
        spellCode = FAIO::read32(exe);
        useOnce = FAIO::read32(exe);
        price1 = FAIO::read32(exe);
        price2 = FAIO::read32(exe);

        itemName = FAIO::readCString(exe, tempName-codeOffset);
        if(tempSecondName)
        {
            itemSecondName = FAIO::readCString(exe, tempSecondName-codeOffset);
        }
        else
        {
            itemSecondName = "";
        }

    }

    std::string BaseItem::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
        << "\tactiveTrigger: " << +activTrigger << "," << std::endl
        << "\titemType: " << +itemType << "," << std::endl
        << "\tequipLoc: " << +equipLoc << "," << std::endl
        << "\tgraphicValue: " << +graphicValue << "," << std::endl
        << "\titemCode: " << +itemCode << "," << std::endl
        << "\tuniqCode: " << +uniqCode << "," << std::endl
        << "\tunknown0: " << +unknown0 << "," << std::endl
        << "\tunknown1: " << +unknown1 << "," << std::endl


        << "\titemName: " << itemName << "," << std::endl
        << "\titemSecondName: " << itemSecondName << "," << std::endl
        << "\tqualityLevel: " << +qualityLevel << "," << std::endl
        << "\tdurability: " << +durability << "," << std::endl
        << "\tminAttackDamage: " << +minAttackDamage << "," << std::endl
        << "\tmaxAttackDamage: " << +maxAttackDamage << "," << std::endl

        << "\tminArmourClass: " << +minArmourClass << "," << std::endl
        << "\tmaxArmourClass: " << +maxArmourClass << "," << std::endl
        << "\treqStr: " << +reqStr << "," << std::endl
        << "\treqMagic: " << +reqMagic << "," << std::endl
        << "\treqDex: " << +reqDex << "," << std::endl
        << "\treqVit: " << +reqVit << "," << std::endl

        << "\tspecialEffect : " << +specialEffect  << "," << std::endl

        << "\tmagicCode: " << +magicCode << "," << std::endl

        << "\tspellCode: " << +spellCode << "," << std::endl
        << "\tuseOnce: " << +useOnce << "," << std::endl

        << "\tprice1: " << +price1 << "," << std::endl

        << "\tprice2: " << +price2 << "," << std::endl

        << "}" << std::endl;
        return ss.str();

    }

}
