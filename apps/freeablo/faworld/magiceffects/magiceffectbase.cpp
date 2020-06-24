#include "magiceffectbase.h"
#include "magiceffect.h"
#include <diabloexe/exemagicitemeffect.h>

namespace FAWorld
{
    MagicEffectBase::MagicEffectBase(const DiabloExe::ExeMagicItemEffect& exeEffect) : mParameter1(exeEffect.mMinEffect), mParameter2(exeEffect.mMaxEffect)
    {
        mDescriptionFormatString = "+Unimplemented magic effect";
    }

    std::unique_ptr<MagicEffect> MagicEffectBase::create() const { return std::make_unique<MagicEffect>(this); }

    MagicEffectBase::~MagicEffectBase() = default;
}