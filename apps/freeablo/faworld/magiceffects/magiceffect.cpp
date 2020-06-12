#include "magiceffect.h"
#include "magiceffectbase.h"

namespace FAWorld
{
    MagicEffect::MagicEffect(const MagicEffectBase* base) : mBase(base) {}

    std::string MagicEffect::getFullDescription() const { return getBase()->mDescriptionFormatString; }
}
