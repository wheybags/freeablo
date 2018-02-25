#include "playerinput.h"

namespace FAWorld
{
    PlayerInput::PlayerInput(const TargetTileData& data, int32_t actorId)
    {
        mData.targetTile = data;
        mType = Type::TargetTile;
        mActorId = actorId;
    }

    PlayerInput::PlayerInput(const DragOverTileData& data, int32_t actorId)
    {
        mData.dragOverTile = data;
        mType = Type::DragOverTile;
        mActorId = actorId;
    }

    PlayerInput::PlayerInput(const TargetActorData& data, int32_t actorId)
    {
        mData.targetActor = data;
        mType = Type::TargetActor;
        mActorId = actorId;
    }

    PlayerInput::PlayerInput(const TargetItemOnFloorData& data, int32_t actorId)
    {
        mData.targetItemOnFloor = data;
        mType = Type::TargetItemOnFloor;
        mActorId = actorId;
    }

    PlayerInput::PlayerInput(const AttackDirectionData& data, int32_t actorId)
    {
        mData.attackDirection = data;
        mType = Type::AttackDirection;
        mActorId = actorId;
    }

    PlayerInput::PlayerInput(const ChangeLevelData& data, int32_t actorId)
    {
        mData.changeLevel = data;
        mType = Type::ChangeLevel;
        mActorId = actorId;
    }
}
