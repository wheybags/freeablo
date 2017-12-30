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

        for (auto& effect : mEffectData)
            for (auto& param : effect)
                param = exe.read32();

        mName = exe.readCStringFromWin32Binary(mNamePtr, codeOffset);
    }
    std::string UniqueItem::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
           << "\tmName: " << mName << "," << std::endl
           << "\tmItemType: " << +mItemType << "," << std::endl
           << "\tmQualityLevel: " << +mQualityLevel << "," << std::endl
           << "\tmNumEffects: " << +mNumEffects << "," << std::endl
           << "\tmGoldValue: " << +mGoldValue << "," << std::endl;
        for (int i = 0; i < 6; ++i)
        {
            ss << "\tmEffect" << i << ": " << mEffectData[i][0] << '\n';
            ss << "\tmMinRange" << i << ": " << mEffectData[i][1] << '\n';
            ss << "\tmMaxRange" << i << ": " << mEffectData[i][2] << '\n';
        }
        ss << "}" << std::endl;
        return ss.str();
    }
}
