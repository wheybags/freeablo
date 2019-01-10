#include "monsterstats.h"
#include "../fasavegame/gameloader.h"
#include "diabloexe/monster.h"

namespace FAWorld
{
    MonsterStats::MonsterStats(const DiabloExe::Monster& monsterStats)
        : minDamage(monsterStats.minDamage), maxDamage(monsterStats.maxDamage), level(monsterStats.level), mExp(monsterStats.exp), drops(monsterStats.drops)
    {
    }

    MonsterStats::MonsterStats(FASaveGame::GameLoader& loader)
    {
        minDamage = loader.load<int32_t>();
        maxDamage = loader.load<int32_t>();
        level = loader.load<int32_t>();
        mExp = loader.load<int32_t>();
        drops = loader.load<int32_t>();
    }

    void MonsterStats::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("MonsterStats", saver);

        saver.save(minDamage);
        saver.save(maxDamage);
        saver.save(level);
        saver.save(mExp);
        saver.save(drops);
    }
}
