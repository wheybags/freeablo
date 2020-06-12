#pragma once
#include <faworld/actorstats.h>
#include <memory>
#include <vector>

namespace FAWorld
{
    class ItemPrefixOrSuffixBase;
    class MagicEffect;

    class ItemPrefixOrSuffix
    {
    public:
        explicit ItemPrefixOrSuffix(const ItemPrefixOrSuffixBase* base);
        void init();
        ~ItemPrefixOrSuffix();

        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);

        const ItemPrefixOrSuffixBase* getBase() { return mBase; }
        std::string getFullDescription() const;
        void apply(MagicStatModifiers& modifiers) const;

    private:
        const ItemPrefixOrSuffixBase* mBase = nullptr;
        std::vector<std::unique_ptr<MagicEffect>> mEffects;
    };
}
