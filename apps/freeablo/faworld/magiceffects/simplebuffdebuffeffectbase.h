#pragma once
#include "magiceffectbase.h"

namespace FAWorld
{
    class SimpleBuffDebuffEffectBase : public MagicEffectBase
    {
        using super = MagicEffectBase;

    public:
        explicit SimpleBuffDebuffEffectBase(const DiabloExe::ExeMagicItemEffect& exeEffect);
        ~SimpleBuffDebuffEffectBase() override;

        std::unique_ptr<MagicEffect> create() const override;

    public:
        enum class Attribute
        {
            Strength,
            Magic,
            Dexterity,
            Vitality,
            ToHit,
            Life,
            Mana,
            Damage,
            Armor,
        };

        Attribute mAttribute = {};
    };
}
