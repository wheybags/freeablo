#ifndef BASEITEM_H
#define BASEITEM_H
#include <stdint.h>

#include <string>
#include <map>
#include <faio/fafileobject.h>



namespace DiabloExe
{
    class BaseItem
    {
    public:
        uint32_t activTrigger;

        uint8_t itemType;
        uint8_t equipLoc;

        uint16_t unknown0;

        uint32_t graphicValue;

        uint8_t itemCode;
        uint8_t uniqCode;
        uint16_t unknown1;

        std::string itemName;
        std::string itemSecondName;
        uint32_t qualityLevel;
        uint32_t durability;
        uint32_t minAttackDamage;
        uint32_t maxAttackDamage;
        uint32_t minArmourClass; // LONG LIVE OUR GLORIOUS QUEEN
        uint32_t maxArmourClass;

        uint8_t  reqStr;
        uint8_t  reqMagic;
        uint8_t  reqDex;
        uint8_t  reqVit;

        uint32_t specialEffect;
        uint32_t magicCode;
        uint32_t spellCode;
        uint32_t useOnce;
        uint32_t price1;
        uint32_t price2;

        std::string dump() const;
        BaseItem();

    private:
        BaseItem(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;


    };



}














#endif // BASEITEM_H
