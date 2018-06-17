#pragma once
#include <cstdint>

namespace Serial
{
    class Saver;
    class Loader;
}

namespace FAWorld
{
    enum class EquipTargetType : uint8_t
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

    class EquipTarget
    {
    public:
        EquipTarget() = default;
        void load(Serial::Loader& loader);
        void save(Serial::Saver& saver) const;

        bool operator<(const EquipTarget& other) const;

    protected:
        EquipTarget(EquipTargetType typeArg, int32_t posXArg = 0, int32_t posYArg = 0) : type(typeArg), posX(posXArg), posY(posYArg) {}

    public:
        EquipTargetType type = EquipTargetType::inventory;
        int32_t posX = 0;
        int32_t posY = 0;
    };

    template <EquipTargetType Location> struct MakeEquipTarget : EquipTarget
    {
        MakeEquipTarget() : EquipTarget(Location) {}
    };

    template <> struct MakeEquipTarget<EquipTargetType::inventory> : EquipTarget
    {
        MakeEquipTarget(int32_t x, int32_t y) : EquipTarget(EquipTargetType::inventory, x, y) {}
    };

    template <> struct MakeEquipTarget<EquipTargetType::belt> : EquipTarget
    {
        MakeEquipTarget(int32_t x) : EquipTarget(EquipTargetType::belt, x) {}
    };
}
