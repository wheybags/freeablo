#pragma once
#include "actor.h"
#include "enums.h"
#include "monster.h"

namespace FAWorld
{
    class PlayerBehaviour;

    class Player : public Actor
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        Player(World& world, PlayerClass playerClass, const DiabloExe::CharacterStats& charStats);
        void initCommon();
        Player(World& world, FASaveGame::GameLoader& loader);
        void save(FASaveGame::GameSaver& saver) const override;

        virtual ~Player();
        void updateSprites() override;
        bool dropItem(const Misc::Point& clickedPoint);

        virtual void update(bool noclip) override;

        PlayerBehaviour* getPlayerBehaviour() { return (PlayerBehaviour*)mBehaviour.get(); }

        PlayerClass getClass() const { return mPlayerClass; }
        virtual bool canCriticalHit() const override { return mPlayerClass == PlayerClass::warrior; }

        bool castSpell(SpellId spell, Misc::Point targetPoint) override;
        void doSpellEffect(SpellId spell, Misc::Point targetPoint) override;
        SpellId defaultSkill() const;

        virtual void calculateStats(LiveActorStats& stats, const ActorStats& actorStats) const override;

        // This isn't serialised as it must be set before saving can occur.
        bool mPlayerInitialised = false;

    protected:
        virtual DamageType getMeleeDamageType() const override;

    private:
        bool canTalkTo(Actor* actor);
        void onEnemyKilled(Actor* enemy) override;
        void addExperience(Actor& enemy);
        void levelUp(int32_t newLevel);

        static BaseStats initialiseActorStats(const DiabloExe::CharacterStats& from);

    private:
        struct CalculateStatsCacheKey
        {
            BaseStats baseStats;
            const GameLevel* gameLevel = nullptr;
            int32_t level = 0;
            int32_t inventoryChangedCallCount = -1;

            bool operator==(const CalculateStatsCacheKey& other)
            {
                return baseStats == other.baseStats && gameLevel == other.gameLevel && level == other.level &&
                       inventoryChangedCallCount == other.inventoryChangedCallCount;
            }
        };
        mutable CalculateStatsCacheKey mLastStatsKey; // not serialised, only used to determine if we need to recalculate stats

        int32_t mInventoryChangedCallCount = 0; // not serialised, only used to determine if inventory changed since we last calculated stats
        PlayerClass mPlayerClass = PlayerClass::warrior;
    };
}
