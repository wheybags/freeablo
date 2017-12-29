
#pragma once

#include <faio/fafileobject.h>
#include <cstdint>
#include <string>
#include <array>

namespace DiabloExe
{
    class DiabloExe;
    class UniqueItem
    {
    public:
        std::string dump() const;
        uint32_t mNamePtr;
        std::string mName;

        uint8_t mItemType;

        uint8_t mQualityLevel;

        uint16_t mNumEffects;

        uint32_t mGoldValue;

        std::array<std::array<uint32_t, 3>, 6> mEffectData;
        UniqueItem() {}

    private:
        UniqueItem(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;
    };
}
