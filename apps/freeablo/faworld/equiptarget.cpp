#include "equiptarget.h"
#include <serial/loader.h>
#include <tuple>

namespace FAWorld
{
    bool EquipTarget::operator<(const EquipTarget& other) const { return std::tie(type, posX, posY) < std::tie(other.type, other.posX, other.posY); }

    void EquipTarget::load(Serial::Loader& loader)
    {
        type = EquipTargetType(loader.load<uint8_t>());
        posX = loader.load<int32_t>();
        posY = loader.load<int32_t>();
    }

    void EquipTarget::save(Serial::Saver& saver) const
    {
        saver.save(uint8_t(type));
        saver.save(posX);
        saver.save(posY);
    }
}
