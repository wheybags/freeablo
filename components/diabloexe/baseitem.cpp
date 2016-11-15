#include "baseitem.h"
#include <faio/faio.h>

#include <iostream>
#include <sstream>

namespace DiabloExe
{

    BaseItem::BaseItem(){}

    BaseItem::BaseItem(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        activTrigger = exe.read32();

        itemType = exe.read8();
        equipLoc = exe.read8();

        unknown0 = exe.read16();

        graphicValue = exe.read32();

        itemCode = exe.read8();
        uniqCode = exe.read8();

        unknown1 = exe.read16();


        uint32_t tempName = exe.read32();
        uint32_t tempSecondName = exe.read32();
        qualityLevel = exe.read32();
        durability = exe.read32();
        minAttackDamage = exe.read32();
        maxAttackDamage = exe.read32();
        minArmourClass  = exe.read32();
        maxArmourClass  = exe.read32();

        reqStr = exe.read8();
        reqMagic = exe.read8();
        reqDex   = exe.read8();
        reqVit   = exe.read8();

        specialEffect = exe.read32();
        magicCode = exe.read32();
        spellCode = exe.read32();
        useOnce = exe.read32();
        price1 = exe.read32();
        price2 = exe.read32();

        itemName = exe.readCStringFromWin32Binary(tempName, codeOffset);
        if(tempSecondName)
        {
            itemSecondName = exe.readCStringFromWin32Binary(tempSecondName, codeOffset);
        }
        else
        {

            itemSecondName = std::string();
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
