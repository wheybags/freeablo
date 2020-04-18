#include "monster.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include "diabloexe/monster.h"
#include "itemfactory.h"
#include <memory>
#include <misc/stringops.h>

namespace FAWorld
{
    const std::string Monster::typeId = "monster";

    Monster::Monster(World& world, const DiabloExe::Monster& monsterData) : Actor(world)
    {
        mStats.initialise(BaseStats());

        mBehaviour = std::make_unique<BasicMonsterBehaviour>(this);
        mFaction = Faction::hell();
        mName = monsterData.monsterName;
        mMonsterId = monsterData.idName;

        mSoundPath = monsterData.soundPath;
        Misc::StringUtils::replace(mSoundPath, "%c", "{}");
        Misc::StringUtils::replace(mSoundPath, "%i", "{}");

        mStats.mLevel = monsterData.level;
        mType = ActorType(monsterData.type);
        mStats.getHp() = Misc::MaxCurrentItem(world.mRng->randomInRange(monsterData.minHp, monsterData.maxHp));
        mMoveHandler.mSpeedTilesPerSecond = DiabloExe::getSpeedByMonsterAttackType(monsterData.attackType);

        commonInit();
    }

    Monster::Monster(World& world, FASaveGame::GameLoader& loader) : Actor(world, loader)
    {
        mMonsterId = loader.load<std::string>();
        commonInit();
    }

    void Monster::commonInit()
    {
        const DiabloExe::Monster& monsterProperties = mWorld.mDiabloExe.getMonster(mMonsterId);
        mMeleeHitFrame = monsterProperties.hitFrame;
        restoreAnimations();

        mInitialised = true;
    }

    void Monster::save(FASaveGame::GameSaver& saver) const
    {
        Serial::ScopedCategorySaver cat("Monster", saver);
        Actor::save(saver);
        saver.save(mMonsterId);
    }

    void Monster::calculateStats(LiveActorStats& stats, const ActorStats& actorStats) const
    {
        if (!mInitialised)
            return;

        CalculateStatsCacheKey statsCacheKey;
        statsCacheKey.baseStats = actorStats.baseStats;
        statsCacheKey.gameLevel = getLevel();
        statsCacheKey.level = actorStats.mLevel;

        if (statsCacheKey == mLastStatsKey)
            return;
        mLastStatsKey = statsCacheKey;

        const DiabloExe::Monster& monsterProperties = mWorld.mDiabloExe.getMonster(mName);

        stats = LiveActorStats(); // clear everything to zero before we start

        stats.maxLife = -1; // don't change
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
        stats.hitRecoveryDamageThreshold = actorStats.mLevel + 3; // https://wheybags.gitlab.io/jarulfs-guide/#monster-timing-information
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

    void Monster::restoreAnimations()
    {
        FARender::SpriteLoader& spriteLoader = FARender::Renderer::get()->mSpriteLoader;
        FARender::SpriteLoader::MonsterSpriteDefinition spriteDefinitions = spriteLoader.mMonsterSpriteDefinitions[mMonsterId];

        mAnimation.setAnimationSprites(AnimState::walk, spriteLoader.getSprite(spriteDefinitions.walk));
        mAnimation.setAnimationSprites(AnimState::idle, spriteLoader.getSprite(spriteDefinitions.idle));
        mAnimation.setAnimationSprites(AnimState::dead, spriteLoader.getSprite(spriteDefinitions.dead));
        mAnimation.setAnimationSprites(AnimState::attack, spriteLoader.getSprite(spriteDefinitions.attack));
        mAnimation.setAnimationSprites(AnimState::hit, spriteLoader.getSprite(spriteDefinitions.hit));

        mAnimation.markAnimationsRestoredAfterGameLoad();
    }
}
