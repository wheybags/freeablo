#pragma once

enum class MagicalItemTargetBitmask
{
    None = 0x0,
    Jewelery = 0x1,
    Bow = 0x10,
    Staff = 0x100,
    OtherWeapons = 0x1000,
    Shield = 0x10000,
    Armor = 0x100000,
};

enum class ItemType
{
    misc = 0,
    sword,
    axe,
    bow,
    mace,
    shield,
    lightArmor,
    helm,
    mediumArmor,
    heavyArmor,
    staff,
    gold,
    ring,
    amulet,
    none = -1,
};

enum class ItemEquipType
{
    none = 0,
    oneHanded = 1,
    twoHanded = 2,
    chest = 3,
    head = 4,
    ring = 5,
    amulet = 6,
};

enum class ItemClass
{
    none = 0,
    weapon,
    armor,
    jewelryAndConsumable,
    gold,
    quest,
};