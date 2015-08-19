#include "characterstats.h"
#include <sstream>


namespace DiabloExe
{
    std::string CharacterStats::dump() const
    {
        std::stringstream ss;
        ss << "{" << std::endl
        << "\tmStrength: " << +mStrength << "," << std::endl
        << "\tmMagic: " << +mMagic << "," << std::endl
        << "\tmDexterity: " << +mDexterity << "," << std::endl
        << "\tmVitality: " << +mVitality << "," << std::endl
        << "\tmMaxStrength: " << +mMaxStrength << "," << std::endl
        << "\tmMaxMagic: " << +mMaxMagic << "," << std::endl
        << "\tmMaxDexterity: " << +mMaxDexterity << "," << std::endl
        << "\tmMaxVitality: " << +mMaxVitality << "," << std::endl
        << "\tmBlockingBonus: " << +mBlockingBonus << "," << std::endl
        << "\tmIdleInDungeonFrameset: " << +mIdleInDungeonFrameset << "," << std::endl
        << "\tmAttackFrameset: " << +mAttackFrameset << "," << std::endl
        << "\tmWalkInDungeonFrameset: " << +mWalkInDungeonFrameset << "," << std::endl
        << "\tmBlockingSpeed: " << +mBlockingSpeed << "," << std::endl
        << "\tmDeathFrameset: " << +mDeathFrameset << "," << std::endl
        << "\tmMagicCastFrameset: " << +mMagicCastFrameset << "," << std::endl
        << "\tmHitRecoverySpeed: " << +mHitRecoverySpeed << "," << std::endl
        << "\tmIdleInTownFrameset: " << +mIdleInTownFrameset << "," << std::endl
        << "\tmWalkInTownFrameset: " << +mWalkInTownFrameset << "," << std::endl
        << "\tmSingleHandedAttackSpeed: " << +mSingleHandedAttackSpeed << "," << std::endl
        << "\tmSpellCastSpeed: " << +mSpellCastSpeed << "," << std::endl
        << "}" << std::endl;
        return ss.str();


    }

}
