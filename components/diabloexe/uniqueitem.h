#ifndef UNIQUEITEM_H
#define UNIQUEITEM_H
#include <stdint.h>
#include <string>
#include <faio/fafileobject.h>

namespace DiabloExe
{
    class DiabloExe;
    class UniqueItem
    {
    public:

        std::string dump() const;
        uint32_t mNamePtr;
        std::string mName;

        uint8_t mItemType;

        uint8_t mQualityLevel;

        uint16_t mNumEffects;

        uint32_t mGoldValue;

        uint32_t mEffect0;
        uint32_t mMinRange0;
        uint32_t mMaxRange0;

        uint32_t mEffect1;
        uint32_t mMinRange1;
        uint32_t mMaxRange1;

        uint32_t mEffect2;
        uint32_t mMinRange2;
        uint32_t mMaxRange2;

        uint32_t mEffect3;
        uint32_t mMinRange3;
        uint32_t mMaxRange3;

        uint32_t mEffect4;
        uint32_t mMinRange4;
        uint32_t mMaxRange4;

        uint32_t mEffect5;
        uint32_t mMinRange5;
        uint32_t mMaxRange5;
        UniqueItem(){}
    private:
        UniqueItem(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;


    };

}
#endif // UNIQUEITEM_H

