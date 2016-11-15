#include "uniqueitem.h"
#include <sstream>
namespace DiabloExe
{

    UniqueItem::UniqueItem(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        mNamePtr = exe.read32();

        mItemType = exe.read8();

        mQualityLevel = exe.read8();

        mNumEffects = exe.read16();

        mGoldValue = exe.read32();

        mEffect0 = exe.read32();
        mMinRange0 = exe.read32();
        mMaxRange0 = exe.read32();

        mEffect1 = exe.read32();
        mMinRange1 = exe.read32();
        mMaxRange1 = exe.read32();

        mEffect2 = exe.read32();
        mMinRange2 = exe.read32();
        mMaxRange2 = exe.read32();

        mEffect3 = exe.read32();
        mMinRange3 = exe.read32();
        mMaxRange3 = exe.read32();

        mEffect4 = exe.read32();
        mMinRange4 = exe.read32();
        mMaxRange4 = exe.read32();

        mEffect5 = exe.read32();
        mMinRange5 = exe.read32();
        mMaxRange5 = exe.read32();

        mName = exe.readCStringFromWin32Binary(mNamePtr, codeOffset);



    }
    std::string UniqueItem::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
           << "\tmName: "         << mName << "," << std::endl
           << "\tmItemType: "     << +mItemType << "," << std::endl
           << "\tmQualityLevel: " << +mQualityLevel << "," << std::endl
           << "\tmNumEffects: "      << +mNumEffects << "," << std::endl
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
