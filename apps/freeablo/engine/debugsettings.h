#pragma once

namespace DebugSettings
{
    extern bool DebugMissiles;
    extern bool DebugLevelTransitions;
    extern bool Instakill;
    extern bool EnemiesFrozen;
    extern bool PlayersInvuln;
    extern bool DisableMusic;

    enum class ItemGenerationType
    {
        Normal,
        AlwaysMagical,
    };

    extern ItemGenerationType itemGenerationType;
}