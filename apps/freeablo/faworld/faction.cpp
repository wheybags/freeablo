#include "faction.h"

namespace FAWorld
{

    bool Faction::canAttack(const Faction& other) const
    {
        return mFaction != other.mFaction;
    }

}
