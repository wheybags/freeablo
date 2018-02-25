#pragma once
#include "itemmap.h"

namespace FAWorld
{
    class GameLevel;
    class HoverStatus
    {
    public:
        HoverStatus() : hoveredActorId(-1), hoveredItemTile(Tile()) {}
        explicit HoverStatus(int32_t actorId) : hoveredActorId(actorId), hoveredItemTile(Tile()) {}
        explicit HoverStatus(Tile itemTile) : hoveredActorId(-1), hoveredItemTile(itemTile) {}
        std::string getDescription(GameLevel& level) const;

        int32_t hoveredActorId;
        Tile hoveredItemTile;
    };
}
