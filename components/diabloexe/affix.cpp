#include "affix.h"
#include <string>
#include <map>
#include <faio/faio.h>
#include <sstream>
namespace DiabloExe
{

    Affix::Affix(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        uint32_t nameTemp = exe.read32();
        mEffect   = exe.read32();
        mMinEffect      = exe.read32();
        mMaxEffect      = exe.read32();
        mQualLevel      = exe.read32();

        mBowjewelProb = exe.read8();
        mWSProb       = exe.read8();
        mASProb       = exe.read8();
        mUnknown0     = exe.read8();

        mExcludedCombination0 = exe.read32();
        mExcludedCombination1 = exe.read32();

        mCursed     = exe.read32();
        mMinGold    = exe.read32();
        mMaxGold    = exe.read32();
        mMultiplier = exe.read32();
        mName     = exe.readCStringFromWin32Binary(nameTemp, codeOffset);



    }

    std::string Affix::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
        << "\tmName: " << mName << std::endl
        << "\tmEffect: " << mEffect << ", " << std::endl
        << "\tmMinEffect: "  << (size_t)mMinEffect << "," << std::endl
        << "\tmMaxEffect: "  << (size_t)mMaxEffect << "," << std::endl
        << "\tmQualEffect: " << (size_t)mQualLevel << "," << std::endl
        << "\tmBowjewelProb: " << (size_t)mBowjewelProb << "," << std::endl
        << "\tmWSProb: " << (size_t)mWSProb << "," << std::endl
        << "\tmASProb: " << (size_t)mASProb << "," << std::endl
        << "\tmUnknown: " << (size_t)mUnknown0 << "," << std::endl
        << "\tmExcludedCombination0: " << (size_t)mExcludedCombination0 << "," << std::endl
        << "\tmExcludedCombination1: " << (size_t)mExcludedCombination1 << "," << std::endl
        << "\tmCursed: " << (size_t)mCursed << "," << std::endl
        << "\tmMinGold: " << (size_t)mMinGold << "," << std::endl
        << "\tmMaxGold: " << (size_t)mMaxGold << "," << std::endl
        << "\tmMultiplier: " << (size_t)mMultiplier << "," << std::endl
        << "}" << std::endl;

        return ss.str();

    }
}
