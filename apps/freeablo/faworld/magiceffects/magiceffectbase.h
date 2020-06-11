#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace DiabloExe
{
    class ExeMagicItemEffect;
}

namespace FAWorld
{
    class MagicEffect;

    class MagicEffectBase
    {
    public:
        explicit MagicEffectBase(const DiabloExe::ExeMagicItemEffect& exeEffect);
        virtual ~MagicEffectBase();

        virtual std::unique_ptr<MagicEffect> create() const;

    public:
        int32_t mParameter1 = 0;
        int32_t mParameter2 = 0;
        std::string mDescriptionFormatString;
    };
}