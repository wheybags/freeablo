#include "hoverstate.h"

namespace FAWorld
{
    class Tile;

    bool HoverState::applyIfNeeded(const HoverState& newState)
    {
        if(*this == newState)
            return false;
        *this = newState;
        return true;
    }

    bool HoverState::setItemHovered(const FAWorld::Tile& tile)
    {
        HoverState newState(HoverType::item);
        newState.mItemTile = tile;
        return applyIfNeeded(newState);
    }

    bool HoverState::isItemHovered(const FAWorld::Tile& tile) const
    {
        return mType == HoverType::item && tile == mItemTile;
    }

    bool HoverState::operator==(const HoverState& other) const
    {
        if(mType != other.mType)
            return false;

        switch(mType)
        {
            case HoverType::actor:
                return mActorId == other.mActorId;
            case HoverType::item:
                return mItemTile == other.mItemTile;
            case HoverType::none:
                break;
        }
        return true;
    }

    bool HoverState::setActorHovered(int32_t actorIdArg)
    {
        HoverState newState(HoverType::actor);
        newState.mActorId = actorIdArg;
        return applyIfNeeded(newState);
    }

    bool HoverState::setNothingHovered()
    {
        HoverState newState(HoverType::none);
        return applyIfNeeded(newState);
    }

    bool HoverState::isActorHovered(int32_t actorId) const
    {
        return mType == HoverType::actor && actorId == mActorId;
    }
 }
