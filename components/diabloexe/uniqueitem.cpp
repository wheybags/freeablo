#include "uniqueitem.h"
#include <sstream>
namespace DiabloExe
{

    UniqueItem::UniqueItem(FAIO::FAFile *exe, size_t codeOffset)
    {
        mNamePtr = FAIO::read32(exe);

        mItemType = FAIO::read8(exe);

        mQualityLevel = FAIO::read8(exe);

        mNumAttr = FAIO::read16(exe);

        mGoldValue = FAIO::read32(exe);

        mEffect0 = FAIO::read32(exe);
        mMinRange0 = FAIO::read32(exe);
        mMaxRange0 = FAIO::read32(exe);

        mEffect1 = FAIO::read32(exe);
        mMinRange1 = FAIO::read32(exe);
        mMaxRange1 = FAIO::read32(exe);

        mEffect2 = FAIO::read32(exe);
        mMinRange2 = FAIO::read32(exe);
        mMaxRange2 = FAIO::read32(exe);

        mEffect3 = FAIO::read32(exe);
        mMinRange3 = FAIO::read32(exe);
        mMaxRange3 = FAIO::read32(exe);

        mEffect4 = FAIO::read32(exe);
        mMinRange4 = FAIO::read32(exe);
        mMaxRange4 = FAIO::read32(exe);

        mEffect5 = FAIO::read32(exe);
        mMinRange5 = FAIO::read32(exe);
        mMaxRange5 = FAIO::read32(exe);

        mName = FAIO::readCStringFromWin32Binary(exe, mNamePtr, codeOffset);



    }
    std::string UniqueItem::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
           << "\tmName: "         << mName << "," << std::endl
           << "\tmItemType: "     << +mItemType << "," << std::endl
           << "\tmQualityLevel: " << +mQualityLevel << "," << std::endl
           << "\tmNumAttr: "      << +mNumAttr << "," << std::endl
           << "\tmGoldValue: "    << +mGoldValue << "," << std::endl
           << "\tmEffect0: "      << +mEffect0 << "," << std::endl
           << "\tmMinRange0: "    << +mMinRange0 << "," << std::endl
           << "\tmMaxRange0: "     << +mMaxRange0 << "," << std::endl
           << "\tmEffect1: "      << +mEffect1 << "," << std::endl
           << "\tmMinRange1: "    << +mMinRange1 << "," << std::endl
           << "\tmMaxRange1: "     << +mMaxRange1 << "," << std::endl
           << "\tmEffect2: "      << +mEffect2 << "," << std::endl
           << "\tmMinRange2: "    << +mMinRange2 << "," << std::endl
           << "\tmMaxRange2: "     << +mMaxRange2 << "," << std::endl
           << "\tmEffect3: "      << +mEffect3 << "," << std::endl
           << "\tmMinRange3: "    << +mMinRange3 << "," << std::endl
           << "\tmMaxRange3: "     << +mMaxRange3 << "," << std::endl
           << "\tmEffect4: "      << +mEffect4 << "," << std::endl
           << "\tmMinRange4: "    << +mMinRange4 << "," << std::endl
           << "\tmMaxRange4: "     << +mMaxRange4 << "," << std::endl
           << "\tmEffect5: "      << +mEffect5 << "," << std::endl
           << "\tmMinRange5: "    << +mMinRange5 << "," << std::endl
           << "\tmMaxRange5: "     << +mMaxRange5 << "," << std::endl
           << "}" << std::endl;
        return ss.str();

    }

}
