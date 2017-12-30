#pragma once

namespace FAWorld
{
    enum class EquipTargetType
    {
        inventory,
        belt,
        head,
        body,
        leftRing,
        rightRing,
        leftHand,
        rightHand,
        amulet,
        cursor,
    };

    struct EquipTarget
    {
    public:
        EquipTarget() = delete;
        bool operator<(const EquipTarget& other) const;

    protected:
        EquipTarget(EquipTargetType typeArg, int posXArg = -1, int posYArg = -1) : type(typeArg), posX(posXArg), posY(posYArg) {}

    public:
        EquipTargetType type;
        int posX;
        int posY;
    };

    template <EquipTargetType Location> struct MakeEquipTarget : EquipTarget
    {
        MakeEquipTarget() : EquipTarget(Location) {}
    };

    template <> struct MakeEquipTarget<EquipTargetType::inventory> : EquipTarget
    {
        MakeEquipTarget(int x, int y) : EquipTarget(EquipTargetType::inventory, x, y) {}
    };

    template <> struct MakeEquipTarget<EquipTargetType::belt> : EquipTarget
    {
        MakeEquipTarget(int x) : EquipTarget(EquipTargetType::belt, x) {}
    };
}
