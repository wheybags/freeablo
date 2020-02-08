#include "monster.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include "diabloexe/monster.h"
#include "itemfactory.h"
#include <fmt/format.h>
#include <misc/stringops.h>
#include <random/random.h>

namespace FAWorld
{
    const std::string Monster::typeId = "monster";

    Monster::Monster(World& world, const DiabloExe::Monster& monsterStats) : Actor(world)
    {
        std::string cl2PathFormat = monsterStats.cl2Path;
        Misc::StringUtils::replace(cl2PathFormat, "%c", "{}");

        mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage(fmt::format(cl2PathFormat, 'w')));
        mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage(fmt::format(cl2PathFormat, 'n')));
        mAnimation.setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage(fmt::format(cl2PathFormat, 'd')));
        mAnimation.setAnimation(AnimState::attack, FARender::Renderer::get()->loadImage(fmt::format(cl2PathFormat, 'a')));
        mAnimation.setAnimation(AnimState::hit, FARender::Renderer::get()->loadImage(fmt::format(cl2PathFormat, 'h')));

        mBehaviour.reset(new BasicMonsterBehaviour(this));
        mFaction = Faction::hell();
        mName = monsterStats.monsterName;

        mSoundPath = monsterStats.soundPath;
        Misc::StringUtils::replace(mSoundPath, "%c", "{}");
        Misc::StringUtils::replace(mSoundPath, "%i", "{}");

        mStats.mLevel = monsterStats.level;
    }

    Monster::Monster(World& world, FASaveGame::GameLoader& loader) : Actor(world, loader) {}

    void Monster::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Monster", saver);
        Actor::save(saver);
    }

    void Monster::calculateStats(LiveActorStats& stats, const ActorStats& actorStats) const
    {
        CalculateStatsCacheKey statsCacheKey;
        memset(&statsCacheKey, 0, sizeof(CalculateStatsCacheKey)); // force all padding to zero, to make sure memcmp will work

        statsCacheKey.baseStats = actorStats.baseStats;
        statsCacheKey.gameLevel = getLevel();
        statsCacheKey.level = actorStats.mLevel;

        // using memcmp because I didn't want to manually implement operator==
        if (memcmp(&statsCacheKey, &mLastStatsKey, sizeof(CalculateStatsCacheKey)) == 0)
            return;

        memcpy(&mLastStatsKey, &statsCacheKey, sizeof(CalculateStatsCacheKey));

        const DiabloExe::Monster& monsterProperties = mWorld.mDiabloExe.getMonster(mName);

        stats = LiveActorStats(); // clear everything to zero before we start

        stats.maxLife = monsterProperties.maxHp; // TODO: should this be randomised per-spawn between minHp and maxHp?
        stats.armorClass = monsterProperties.armourClass;

        stats.toHitMelee.base = int32_t(monsterProperties.toHit) + 2 * int32_t(monsterProperties.level);
        stats.toHitMelee.bonus = 30;

        const GameLevel* dungeonLevel = getLevel();
        release_assert(dungeonLevel);

        if (dungeonLevel->getLevelIndex() >= 16)
            stats.toHitMeleeMinMaxCap.min = 30;
        else if (dungeonLevel->getLevelIndex() >= 15)
            stats.toHitMeleeMinMaxCap.min = 25;
        else if (dungeonLevel->getLevelIndex() >= 14)
            stats.toHitMeleeMinMaxCap.min = 20;
        else
            stats.toHitMeleeMinMaxCap.min = 15;

        stats.meleeDamageBonusRange = IntRange(monsterProperties.minDamage, monsterProperties.maxDamage);
        stats.hitRecoveryDamageThreshold = actorStats.mLevel + 3; // Jarulf's guide, section 5.3.4
    }

    void Monster::die()
    {
        super::die();
        spawnItem();
    }

    int32_t Monster::getOnKilledExperience() const { return mWorld.mDiabloExe.getMonster(mName).exp; }

    void Monster::spawnItem()
    {
        // TODO: Spawn magic, unique and special/quest items.
        ItemId itemId = randomItem();
        if (itemId < ItemId::COUNT)
        {
            Item item = mWorld.getItemFactory().generateBaseItem(itemId);
            getLevel()->dropItemClosestEmptyTile(item, *this, getPos().current(), Misc::Direction(Misc::Direction8::none));
        }
    }

    ItemId Monster::randomItem()
    {
        if (mWorld.mRng->randomInRange(0, 99) > 40)
            // No drop.
            return ItemId::COUNT;

        if (mWorld.mRng->randomInRange(0, 99) > 25)
            return ItemId::gold;

        return mWorld.getItemFactory().randomItemId(ItemFilter::maxQLvl(mStats.mLevel));
    }
}
