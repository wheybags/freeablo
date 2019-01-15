#pragma once

#include <cstdint>
#include <vector>

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

        int32_t expToLevel(uint32_t exp) const;
        uint32_t nextLevelExp() const;
        uint32_t maxExp() const;

    public:
        int32_t mStrength = 1;
        int32_t mDexterity = 1;
        int32_t mMagic = 1;
        int32_t mVitality = 1;
        uint32_t mExp = 0;
        int32_t mLevel = 1;
        std::vector<uint32_t> mNextLevelExp;
    };
}
