#pragma once

#include "actor.h"
#include "monsterstats.h"

namespace FAWorld
{
    class Monster : public Actor
    {
    public:
        Monster(World& world, Random::Rng& rng, const DiabloExe::Monster& monsterStats);
        Monster(World& world, FASaveGame::GameLoader& loader);

        void save(FASaveGame::GameSaver& saver) override;

        const MonsterStats& getMonsterStats() const { return mMonsterStats; }
        int32_t meleeDamageVs(const Actor* actor) const override;
        int32_t getKillExp() const;

    private:
        MonsterStats mMonsterStats;
    };
}
