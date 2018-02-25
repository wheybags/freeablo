#pragma once

#include <cstdint>

namespace DiabloExe
{
    class CharacterStats;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class PlayerStats
    {
    public:
        PlayerStats() = default;
        PlayerStats(const DiabloExe::CharacterStats& charStats);
        PlayerStats(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

    public:
        int32_t mStrength = 1;
        int32_t mDexterity = 1;
        int32_t mMagic = 1;
        int32_t mVitality = 1;
    };
}
