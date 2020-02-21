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
        // Looks like these all are actually frame counts for animations and not required to be read at all
        // The only thing needed is attack frame by weapon/shield equipped and it can't be extracted from exe nicely
        // since it's hardcoded
        uint8_t mIdleInDungeonFrameset = 0;
        uint8_t mAttackFrameset = 0;
        uint8_t mWalkInDungeonFrameset = 0;
        uint8_t mBlockingSpeed = 0;
        uint8_t mDeathFrameset = 0;
        uint8_t mMagicCastFrameset = 0;
        uint8_t mHitRecoverySpeed = 0;
        uint8_t mIdleInTownFrameset = 0;
        uint8_t mWalkInTownFrameset = 0;
        uint8_t mSingleHandedAttackSpeed = 0;
        uint8_t mSpellCastSpeed = 0;

        uint32_t mStrength = 0;
        uint32_t mMagic = 0;
        uint32_t mDexterity = 0;
        uint32_t mVitality = 0;

        uint32_t mBlockingBonus = 0;
        uint32_t mMaxStrength = 0;
        uint32_t mMaxMagic = 0;
        uint32_t mMaxDexterity = 0;
        uint32_t mMaxVitality = 0;
        std::vector<uint32_t> mNextLevelExp;
    };
}
