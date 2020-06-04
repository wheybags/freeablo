#include "exemagicitemeffect.h"
#include <sstream>
#include <string>

namespace DiabloExe
{
    ExeMagicItemEffect::ExeMagicItemEffect(FAIO::FAFileObject& exe, size_t codeOffset)
    {
        uint32_t nameTemp = exe.read32();

        mEffect = exe.read32();
        mMinEffect = exe.read32();
        mMaxEffect = exe.read32();
        mQualLevel = exe.read32();
        mTargetTypeBitmask = MagicalItemType(exe.read32());
        mCompatibilityBitmask = CompatibilityBitMask(exe.read32());
        mDoubleProbabilityForPrefixes = bool(exe.read32());
        mNotCursed = bool(exe.read32());
        mMinGold = exe.read32();
        mMaxGold = exe.read32();
        mGoldMultiplier = exe.read32();

        mName = exe.readCStringFromWin32Binary(nameTemp, codeOffset);
        mEffectType = (!mName.empty() && std::isupper(mName[0])) ? MagicalEffectType::Prefix : MagicalEffectType::Suffix;
    }

    std::string ExeMagicItemEffect::dump() const
    {
        std::stringstream ss;

        std::string targetTypeBitmaskString;
        {
            if (int32_t(mTargetTypeBitmask) & int32_t(MagicalItemType::Jewelery))
                ss << "Jewelery | ";
            if (int32_t(mTargetTypeBitmask) & int32_t(MagicalItemType::Bow))
                ss << "Bow | ";
            if (int32_t(mTargetTypeBitmask) & int32_t(MagicalItemType::Staff))
                ss << "Staff | ";
            if (int32_t(mTargetTypeBitmask) & int32_t(MagicalItemType::OtherWeapons))
                ss << "OtherWeapons | ";
            if (int32_t(mTargetTypeBitmask) & int32_t(MagicalItemType::Shield))
                ss << "Shield | ";
            if (int32_t(mTargetTypeBitmask) & int32_t(MagicalItemType::Armor))
                ss << "Armor | ";

            targetTypeBitmaskString = ss.str();
            if (targetTypeBitmaskString.empty())
                targetTypeBitmaskString = "None";
            else
                targetTypeBitmaskString = targetTypeBitmaskString.substr(0, targetTypeBitmaskString.size() - 3);
        }

        ss.str("");
        ss << "{" << std::endl
           << "\tmName: " << mName << std::endl
           << "\tmEffectType: " << (mEffectType == MagicalEffectType::Prefix ? "Prefix" : "Suffix") << std::endl
           << "\tmEffect: " << mEffect << ", " << std::endl
           << "\tmMinEffect: " << mMinEffect << "," << std::endl
           << "\tmMaxEffect: " << mMaxEffect << "," << std::endl
           << "\tmQualLevel: " << mQualLevel << "," << std::endl
           << "\tmTargetTypeBitmask: " << targetTypeBitmaskString << "," << std::endl
           << "\tmCompatibilityBitmask: " << int32_t(mCompatibilityBitmask) << "," << std::endl
           << "\tmDoubleProbabilityForPrefixes: " << mDoubleProbabilityForPrefixes << "," << std::endl
           << "\tmNotCursed: " << mNotCursed << "," << std::endl
           << "\tmMinGold: " << mMinGold << "," << std::endl
           << "\tmMaxGold: " << mMaxGold << "," << std::endl
           << "\tmGoldMultiplier: " << mGoldMultiplier << "," << std::endl
           << "}" << std::endl;

        return ss.str();
    }
}
