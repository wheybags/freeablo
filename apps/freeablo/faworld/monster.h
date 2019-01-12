#pragma once

#include "actor.h"
#include "item.h"
#include "monsterstats.h"

namespace FAWorld
{
    class Monster : public Actor
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Monster(World& world, Random::Rng& rng, const DiabloExe::Monster& monsterStats);
        Monster(World& world, FASaveGame::GameLoader& loader);

        void save(FASaveGame::GameSaver& saver) override;

        const MonsterStats& getMonsterStats() const { return mMonsterStats; }
        int32_t meleeDamageVs(const Actor* actor) const override;
        void die() override;
        int32_t getKillExp() const;

    private:
        void spawnItem();
        ItemId randomItem();

        MonsterStats mMonsterStats;
    };
}
