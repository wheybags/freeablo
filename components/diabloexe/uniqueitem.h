#pragma once
#include <array>
#include <cstdint>
#include <faio/fafileobject.h>
#include <string>

namespace DiabloExe
{
    class DiabloExe;
    class UniqueItem
    {
    public:
        std::string dump() const;
        uint32_t mNamePtr;
        std::string mName;
        uint8_t mUniqueBaseItemId;
        uint8_t mQualityLevel;
        uint16_t mNumEffects;
        uint32_t mPrice;
        std::array<std::array<uint32_t, 3>, 6> mEffectData;
        UniqueItem() {}

    private:
        UniqueItem(FAIO::FAFileObject& exe, size_t codeOffset);
        friend class DiabloExe;
    };
}
