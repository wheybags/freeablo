
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
namespace DiabloExe
{
    class CharacterStats
    {
    public:
        CharacterStats() {}

        std::string dump() const;
        uint8_t mIdleInDungeonFrameset;
        uint8_t mAttackFrameset;
        uint8_t mWalkInDungeonFrameset;
        uint8_t mBlockingSpeed;
        uint8_t mDeathFrameset;
        uint8_t mMagicCastFrameset;
        uint8_t mHitRecoverySpeed;
        uint8_t mIdleInTownFrameset;
        uint8_t mWalkInTownFrameset;
        uint8_t mSingleHandedAttackSpeed;
        uint8_t mSpellCastSpeed;

        uint32_t mStrength;
        uint32_t mMagic;
        uint32_t mDexterity;
        uint32_t mVitality;

        uint32_t mBlockingBonus;
        uint32_t mMaxStrength;
        uint32_t mMaxMagic;
        uint32_t mMaxDexterity;
        uint32_t mMaxVitality;
        std::vector<uint32_t> mNextLevelExp;
    };
}

