#pragma once
#include "target.h"
#include <cstdint>
#include <misc/direction.h>

namespace FAWorld
{
    class PlayerInput
    {
    public:
        struct TargetTileData
        {
            int32_t x, y;
        };
        struct DragOverTileData
        {
            int32_t x, y;
        };
        struct TargetActorData
        {
            int32_t actorId;
        };
        struct TargetItemOnFloorData
        {
            int32_t x, y;
            Target::ItemTarget::ActionType type;
        };
        struct AttackDirectionData
        {
            Misc::Direction direction;
        };
        struct ChangeLevelData
        {
            enum class Direction
            {
                Up,
                Down
            };

            Direction direction;
        };

        enum class Type : uint8_t
        {
            TargetTile,
            DragOverTile,
            TargetActor,
            TargetItemOnFloor,
            AttackDirection,
            ChangeLevel
        };

        PlayerInput(const TargetTileData& data, int32_t actorId);
        PlayerInput(const DragOverTileData& data, int32_t actorId);
        PlayerInput(const TargetActorData& data, int32_t actorId);
        PlayerInput(const TargetItemOnFloorData& data, int32_t actorId);
        PlayerInput(const AttackDirectionData& data, int32_t actorId);
        PlayerInput(const ChangeLevelData& data, int32_t actorId);

        union
        {
            TargetTileData targetTile;
            DragOverTileData dragOverTile;
            TargetActorData targetActor;
            TargetItemOnFloorData targetItemOnFloor;
            AttackDirectionData attackDirection;
            ChangeLevelData changeLevel;
        } mData;

        Type mType;
        int32_t mActorId;
    };
}
