
#pragma once

#include <misc/maxcurrentitem.h>
#include <stdint.h>

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
        int32_t strength = 0;
        int32_t magic = 0;
        int32_t dexterity = 0;
        int32_t vitality = 0;
    };

    struct ItemStats
    {
        BaseStats baseStats;
        int32_t maxLife = 0;
        int32_t maxMana = 0;
        int32_t armorClass = 0;
        int32_t toHit = 0;
    };

    struct ToHitChance
    {
        int32_t base = 0;
        int32_t bonus = 0;
    };

    struct LiveActorStats
    {
        BaseStats baseStats;
        int32_t maxLife = 0;
        int32_t maxMana = 0;
        int32_t armorClass = 0;
        ToHitChance toHitMelee;
        ToHitChance toHitRanged;
        ToHitChance toHitMagic;
    };

    class Actor;
    class ActorStats
    {
    public:
        ActorStats(const Actor& actor) : mHp(100), mMana(100), mActor(actor) {}

        ActorStats(const Actor& actor, FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver);

        void takeDamage(int32_t damage) { mHp.add(-damage); }
        int32_t getAttackDamage() { return mAttackDamage; }

        BaseStats baseStats;
        Misc::MaxCurrentItem<int32_t> mHp;
        Misc::MaxCurrentItem<int32_t> mMana;

        int32_t mLevel = 1;
        int32_t mExperience = 0;
        int32_t nextLevelExperience() const { return 1; };

        int32_t mAttackDamage = 3;

        void recalculateStats();
        const LiveActorStats& getCalculatedStats() const { return mCalculatedStats; }

    private:
        const Actor& mActor;
        LiveActorStats mCalculatedStats;
        bool mHasBeenCalculated = false;
    };
}
