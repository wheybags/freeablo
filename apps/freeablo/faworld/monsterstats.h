#pragma once

#include <cstdint>

namespace DiabloExe
{
    class Monster;
}

namespace FASaveGame
{
    class GameLoader;
    class GameSaver;
}

namespace FAWorld
{
    class MonsterStats
    {
    public:
        MonsterStats(const DiabloExe::Monster& monsterStats);
        MonsterStats(FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

    public:
        int32_t minDamage;
        int32_t maxDamage;
        int32_t level;
        int32_t mExp;
        int32_t drops;
    };
}
