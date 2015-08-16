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
        << "}" << std::endl;
        return ss.str();


    }

}
