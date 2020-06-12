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
        ~SimpleBuffDebuffEffect() override = default;
        void init() override;

        void save(FASaveGame::GameSaver& saver) const override;
        void load(FASaveGame::GameLoader& loader) override;

        void apply(MagicStatModifiers& modifiers) const override;
        std::string getFullDescription() const override;

        const SimpleBuffDebuffEffectBase* getBase() const;

    private:
        int32_t mValue = 0;
    };
}
