#pragma once
#include <cstdint>

namespace FAWorld
{
    class Actor;

    enum class FactionType : uint8_t
    {
        hell,
        heaven,
        ENUM_END
    };

    // We will use a simple class with a type flag, having this in
    // "true" OO-style with one class for faction is not worth the
    // fuss yet as the logic is simple
    class Faction
    {
    public:
        Faction() = default;
        Faction(FactionType faction) : mFaction(faction){};

        bool canAttack(const Faction& other) const;

        static Faction hell() { return Faction(FactionType::hell); }

        static Faction heaven() { return Faction(FactionType::heaven); }

        FactionType getType() const { return mFaction; }

    private:
        FactionType mFaction;
    };
}
