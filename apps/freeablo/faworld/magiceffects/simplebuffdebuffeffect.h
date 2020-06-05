#pragma once
#include "magiceffect.h"

namespace FAWorld
{
    class SimpleBuffDebuffEffectBase;

    class SimpleBuffDebuffEffect : public MagicEffect
    {
        using super = MagicEffect;

    public:
        explicit SimpleBuffDebuffEffect(const SimpleBuffDebuffEffectBase* base);
        void init() override;

        void apply(MagicStatModifiers& modifiers) override;

        const SimpleBuffDebuffEffectBase* getBase() const;

    private:
        int32_t mValue = 0;
    };
}
