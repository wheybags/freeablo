#include "affix.h"
#include <string>
#include <map>
#include <faio/faio.h>
#include <sstream>
namespace DiabloExe
{

    Affix::Affix(FAIO::FAFile *exe, size_t codeOffset)
    {
        uint32_t nameTemp = FAIO::read32(exe);
        mEffect   = FAIO::read32(exe);
        mMinEffect      = FAIO::read32(exe);
        mMaxEffect      = FAIO::read32(exe);
        mQualLevel      = FAIO::read32(exe);

        mBowjewelProb = FAIO::read8(exe);
        mWSProb       = FAIO::read8(exe);
        mASProb       = FAIO::read8(exe);
        mUnknown0     = FAIO::read8(exe);

        mExcludedCombination0 = FAIO::read32(exe);
        mExcludedCombination1 = FAIO::read32(exe);

        mCursed     = FAIO::read32(exe);
        mMinGold    = FAIO::read32(exe);
        mMaxGold    = FAIO::read32(exe);
        mMultiplier = FAIO::read32(exe);
        mName     = FAIO::readCStringFromWin32Binary(exe, nameTemp, codeOffset);



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
