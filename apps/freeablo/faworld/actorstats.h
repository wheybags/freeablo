#pragma once
#include "world.h"
#include <misc/maxcurrentitem.h>
#include <misc/misc.h>
#include <misc/simplevec2.h>
#include <stdint.h>
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
    struct BaseStats
    {
        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);

        int32_t strength = 0;
        int32_t magic = 0;
        int32_t dexterity = 0;
        int32_t vitality = 0;

        bool operator==(const BaseStats& other)
        {
            return strength == other.strength && magic == other.magic && dexterity == other.dexterity && vitality == other.vitality;
        }
    };

    struct ItemStats
    {
        BaseStats baseStats;
        int32_t maxLife = 0;
        int32_t maxMana = 0;
        int32_t armorClass = 0;
        int32_t toHit = 0;
        IntRange meleeDamageBonusRange = {0, 0};
        IntRange rangedDamageBonusRange = {0, 0};
    };

    struct ToHitChance
    {
        int32_t base = 0;
        int32_t bonus = 0;

        int32_t getCombined() const { return base + bonus; }
    };

    struct LiveActorStats
    {
        BaseStats baseStats;
        int32_t maxLife = 0;
        int32_t maxMana = 0;
        int32_t armorClass = 0;
        ToHitChance toHitMelee;
        IntRange toHitMeleeMinMaxCap = {0, 100};
        ToHitChance toHitRanged;
        ToHitChance toHitMagic;
        int32_t meleeDamage = 0;
        int32_t rangedDamage = 0;
        IntRange meleeDamageBonusRange = {0, 0};
        IntRange rangedDamageBonusRange = {0, 0};
        Tick meleeAttackSpeedInTicks = 0;
        Tick rangedAttackSpeedInTicks = 0;
        Tick spellAttackSpeedInTicks = 0;
        int32_t hitRecoveryDamageThreshold = 0;
        int32_t blockChance = 0;
    };

    class Actor;
    class ActorStats
    {
    public:
        ActorStats(const Actor& actor) : mActor(actor) {}

        void initialise(const BaseStats& baseStats);

        ActorStats(const Actor& actor, FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const;

        ActorStats& operator=(const ActorStats& other) = default;

        void takeDamage(int32_t damage) { mHp.add(-damage); }

        int32_t nextLevelExperience() const { return mLevelXpCounts.at(mLevel - 1); };
        int32_t experiencePointsToLevel(uint32_t experience) const;

        const LiveActorStats& getCalculatedStats() const;

        static constexpr int32_t MAXIMUM_EXPERIENCE_POINTS = 2000000000;

        Misc::MaxCurrentItem<int32_t>& getHp()
        {
            recalculateStats();
            return mHp;
        }
        const Misc::MaxCurrentItem<int32_t>& getHp() const
        {
            recalculateStats();
            return mHp;
        }

        Misc::MaxCurrentItem<int32_t>& getMana()
        {
            recalculateStats();
            return mMana;
        }
        const Misc::MaxCurrentItem<int32_t>& getMana() const
        {
            recalculateStats();
            return mMana;
        }

    private:
        void recalculateStats() const;

    public:
        BaseStats baseStats;
        std::vector<uint32_t> mLevelXpCounts;

        int32_t mLevel = 1;
        int32_t mExperience = 0;

        NonNullConstPtr<Actor> mActor; // not serialised
    private:
        mutable Misc::MaxCurrentItem<int32_t> mHp;
        mutable Misc::MaxCurrentItem<int32_t> mMana;

        mutable LiveActorStats mCalculatedStats; // not serialised, this is recalculated on access
    };
}
