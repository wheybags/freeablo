#ifndef FACTION_H
#define FACTION_H

namespace FAWorld
{
    class Actor;

    enum FactionType
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
        Faction(FactionType faction): mFaction(faction) {};

        bool canAttack(const Faction& other) const;

        static Faction hell()
        {
            return Faction(FactionType::hell);
        }

        static Faction heaven()
        {
            return Faction(FactionType::heaven);
        }

    private:
        FactionType mFaction;
    };

}

#endif
