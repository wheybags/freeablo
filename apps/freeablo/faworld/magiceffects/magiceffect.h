#pragma once
#include <misc/misc.h>

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace FAWorld
{
    class MagicEffectBase;
    struct MagicStatModifiers;

    class MagicEffect
    {
    public:
        explicit MagicEffect(const MagicEffectBase* base);
        virtual ~MagicEffect() = default;
        virtual void init(){};

        virtual void save(FASaveGame::GameSaver& saver) const { UNUSED_PARAM(saver); };
        virtual void load(FASaveGame::GameLoader& loader) { UNUSED_PARAM(loader); };

        virtual void apply(MagicStatModifiers& modifiers) const { UNUSED_PARAM(modifiers); }
        virtual std::string getFullDescription() const;

        const MagicEffectBase* getBase() const { return mBase; }

    protected:
        const MagicEffectBase* mBase = nullptr;
    };
}
