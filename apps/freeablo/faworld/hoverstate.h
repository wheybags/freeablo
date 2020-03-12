#pragma once
#include "itemmap.h"

namespace FAWorld
{
    class GameLevel;
    class HoverStatus
    {
    public:
        HoverStatus() = default;
        explicit HoverStatus(int32_t actorId) : hoveredActorId(actorId) {}
        explicit HoverStatus(Misc::Point itemTile) : hoveredActorId(-1), hoveredItemTile(itemTile) {}
        std::string getDescription(GameLevel& level) const;

        int32_t hoveredActorId = -1;
        Misc::Point hoveredItemTile = Misc::Point::invalid();
    };
}
