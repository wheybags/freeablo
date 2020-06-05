#pragma once
#include <misc/misc.h>

namespace FAWorld
{
    class MagicEffectBase;
    struct MagicStatModifiers;

    class MagicEffect
    {
    public:
        explicit MagicEffect(const MagicEffectBase* base);
        virtual void init(){};

        virtual void apply(MagicStatModifiers& modifiers) { UNUSED_PARAM(modifiers); }

        const MagicEffectBase* getBase() const { return mBase; }

    protected:
        const MagicEffectBase* mBase = nullptr;
    };
}
