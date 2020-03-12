#pragma once
#include <faio/fafileobject.h>
#include <map>
#include <stdint.h>
#include <string>

namespace DiabloExe
{
    // Affix -- Part of a word or phrase can be before or after the object Prefix for before Suffix for afterwords.
    class Affix
    {
    public:
        std::string mName;

        uint32_t mEffect;
        uint32_t mMinEffect;
        uint32_t mMaxEffect;
        uint32_t mQualLevel;
        uint8_t mBowjewelProb;
        uint8_t mWSProb;
        uint8_t mASProb;
        uint8_t mUnknown0;
        uint32_t mExcludedCombination0;
        uint32_t mExcludedCombination1;
        uint32_t mCursed;
        uint32_t mMinGold;
        uint32_t mMaxGold;
        uint32_t mMultiplier;

        std::string dump() const;
        Affix() {}

    private:
        Affix(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;
    };
}
