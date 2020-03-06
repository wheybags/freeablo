#pragma once
#include <cstdint>
#include <misc/assert.h>
#include <string_view>

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

    enum class PlayerClass
    {
        warrior = 0,
        rogue,
        sorceror,
        none,
    };

    // note that this function features misspelling of sorcerer as sorceror
    // because it's written this way on character panel
    const char* playerClassToString(PlayerClass value);
    PlayerClass playerClassFromString(std::string_view str);
}
