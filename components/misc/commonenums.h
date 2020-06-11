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