#pragma once
#include "actor.h"
#include "item.h"

namespace FAWorld
{
    class Monster : public Actor
    {
        using super = Actor;

    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Monster(World& world, const DiabloExe::Monster& monsterStats);
        Monster(World& world, FASaveGame::GameLoader& loader);

        void save(FASaveGame::GameSaver& saver) const override;

        virtual void calculateStats(LiveActorStats& stats, const ActorStats& actorStats) const override;
        void die() override;
        virtual int32_t getOnKilledExperience() const override;

    private:
        void commonInit();
        void spawnItem();
        ItemId randomItem();

    private:
        struct CalculateStatsCacheKey
        {
            BaseStats baseStats;
            const GameLevel* gameLevel = nullptr;
            int32_t level = 0;

            bool operator==(const CalculateStatsCacheKey& other)
            {
                return baseStats == other.baseStats && gameLevel == other.gameLevel && level == other.level;
            }
        };
        mutable CalculateStatsCacheKey mLastStatsKey = {}; // not serialised, only used to determine if we need to recalculate stats

        bool mInitialised = false; // not serialised
    };
}
