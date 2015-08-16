#ifndef CHARACTERSTATS_H
#define CHARACTERSTATS_H
#include <stdint.h>
#include <string>

namespace DiabloExe
{
    class CharacterStats
    {
    public:
        CharacterStats(){}

        std::string dump() const;
        uint32_t mStrength;
        uint32_t mMagic;
        uint32_t mDexterity;
        uint32_t mVitality;

        uint32_t mMaxStrength;
        uint32_t mMaxMagic;
        uint32_t mMaxDexterity;
        uint32_t mMaxVitality;
        uint32_t mBlockingBonus;

    };
}
#endif // CHARACTERSTATS_H
