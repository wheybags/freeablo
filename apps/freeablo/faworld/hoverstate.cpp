#include "hoverstate.h"
#include "actor.h"
#include "gamelevel.h"
#include "world.h"

namespace FAWorld
{
    std::string HoverStatus::getDescription(GameLevel& level) const
    {
        if (hoveredActorId != -1)
            return level.getWorld()->getActorById(hoveredActorId)->getName();

        if (hoveredItemTile.isValid())
        {
            auto item = level.getItemMap().getItemAt(hoveredItemTile);

            // we might have already picked the item up during this tick
            if (item)
                return item->item().getFullDescription();
        }

        return "";
    }
}
