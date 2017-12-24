#include "equiptarget.h"

#include <tuple>

namespace FAWorld
{
    bool EquipTarget::operator<(const EquipTarget& other) const { return std::tie(type, posX, posY) < std::tie(other.type, other.posX, other.posY); }
}
