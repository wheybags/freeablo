#pragma once
#include <cstdint>

namespace FAWorld
{
    enum class ActorType : uint8_t
    {
        // Undead, Demon and Animal set up to match the values from Diablo
        Undead = 0,
        Demon = 1,
        Animal = 2,
        Normal
    };

    enum class DamageType
    {
        Sword,
        Club,
        Axe,
        Bow,
        Staff,
        Unarmed,
    };
}
