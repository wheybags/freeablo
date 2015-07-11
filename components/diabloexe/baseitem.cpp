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

        unknown0 = FAIO::read8(exe);
        unknown1 = FAIO::read8(exe);

        graphicValue = FAIO::read32(exe);

        itemCode = FAIO::read8(exe);
        uniqCode = FAIO::read8(exe);

        unknown2 = FAIO::read8(exe);
        unknown3 = FAIO::read8(exe);


        uint32_t tempName = FAIO::read32(exe);
        uint32_t tempSecondName = FAIO::read32(exe);
        itemName = FAIO::readCString(exe, tempName-codeOffset);
        //std::cout << "namePtr: " << (size_t)tempName << std::endl;
        if(tempSecondName)
        {
            itemSecondName = FAIO::readCString(exe, tempSecondName-codeOffset);
        }
        else
        {
            itemSecondName = "";
        }
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

    }

    std::string BaseItem::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
        << "\tactiveTrigger: " << (size_t)activTrigger << "," << std::endl
        << "\titemType: " << (size_t)itemType << "," << std::endl
        << "\tequipLoc: " << (size_t)equipLoc << "," << std::endl
        << "\tgraphicValue: " << (size_t)graphicValue << "," << std::endl
        << "\titemCode: " << (size_t)itemCode << "," << std::endl
        << "\tuniqCode: " << (size_t)uniqCode << "," << std::endl
        << "\tunknown0: " << (size_t)unknown0 << "," << std::endl
        << "\tunknown1: " << (size_t)unknown1 << "," << std::endl
        << "\tunknown2: " << (size_t)unknown2 << "," << std::endl
        << "\tunknown3: " << (size_t)unknown3 << "," << std::endl


        << "\titemName: " << itemName << "," << std::endl
        << "\titemSecondName: " << itemSecondName << "," << std::endl
        << "\tqualityLevel: " << (size_t)qualityLevel << "," << std::endl
        << "\tdurability: " << (size_t)durability << "," << std::endl
        << "\tminAttackDamage: " << (size_t)minAttackDamage << "," << std::endl
        << "\tmaxAttackDamage: " << (size_t)maxAttackDamage << "," << std::endl

        << "\tminArmourClass: " << (size_t)minArmourClass << "," << std::endl
        << "\tmaxArmourClass: " << (size_t)maxArmourClass << "," << std::endl
        << "\treqStr: " << (size_t)reqStr << "," << std::endl
        << "\treqMagic: " << (size_t)reqMagic << "," << std::endl
        << "\treqDex: " << (size_t)reqDex << "," << std::endl
        << "\treqVit: " << (size_t)reqVit << "," << std::endl

        << "\tspecialEffect : " << (size_t)specialEffect  << "," << std::endl

        << "\tmagicCode: " << (size_t)magicCode << "," << std::endl

        << "\tspellCode: " << (size_t)spellCode << "," << std::endl
        << "\tuseOnce: " << (size_t)useOnce << "," << std::endl

        << "\tprice1: " << (size_t)price1 << "," << std::endl

        << "\tprice2: " << (size_t)price2 << "," << std::endl

        << "}" << std::endl;
        return ss.str();

    }

}
