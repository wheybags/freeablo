#include "monster.h"
#include "../fasavegame/gameloader.h"
#include "actor.h"
#include "diabloexe/monster.h"
#include "itemfactory.h"
#include <boost/format.hpp>
#include <random/random.h>

namespace FAWorld
{
    const std::string Monster::typeId = "monster";

    Monster::Monster(World& world, Random::Rng& rng, const DiabloExe::Monster& monsterStats) : Actor(world), mMonsterStats(monsterStats)
    {
        boost::format fmt(monsterStats.cl2Path);
        mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage((fmt % 'w').str()));
        mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage((fmt % 'n').str()));
        mAnimation.setAnimation(AnimState::dead, FARender::Renderer::get()->loadImage((fmt % 'd').str()));
        mAnimation.setAnimation(AnimState::attack, FARender::Renderer::get()->loadImage((fmt % 'a').str()));
        mAnimation.setAnimation(AnimState::hit, FARender::Renderer::get()->loadImage((fmt % 'h').str()));

        mBehaviour.reset(new BasicMonsterBehaviour(this));
        mFaction = Faction::hell();
        mName = monsterStats.monsterName;
        mSoundPath = monsterStats.soundPath;
        mStats.mHp.max = monsterStats.maxHp;
        mStats.mHp.current = rng.randomInRange(monsterStats.minHp, monsterStats.maxHp);
    }

    Monster::Monster(World& world, FASaveGame::GameLoader& loader) : Actor(world, loader), mMonsterStats(loader) {}

    void Monster::save(FASaveGame::GameSaver& saver)
    {
        Serial::ScopedCategorySaver cat("Monster", saver);
        Actor::save(saver);
        mMonsterStats.save(saver);
    }

    int32_t Monster::meleeDamageVs(const Actor* actor) const
    {
        (void)actor;
        int32_t damage = mWorld.mRng->randomInRange(mMonsterStats.minDamage, mMonsterStats.maxDamage);
        return damage;
    }

    void Monster::die()
    {
        Actor::die();
        spawItem();
    }

    int32_t Monster::getKillExp() const { return mMonsterStats.mExp; }

    void Monster::spawItem()
    {
        // TODO: Spawn magic, unique and special/quest items.
        ItemId itemId = randomItem();
        if (itemId < ItemId::COUNT)
        {
            Item item = mWorld.getItemFactory().generateBaseItem(itemId);
            getLevel()->dropItemClosestEmptyTile(item, *this, getPos().current(), Misc::Direction::none);
        }
    }

    ItemId Monster::randomItem()
    {
        if (mWorld.mRng->randomInRange(0, 99) > 40)
            // No drop.
            return ItemId::COUNT;

        if (mWorld.mRng->randomInRange(0, 99) > 25)
            return ItemId::gold;

        return mWorld.getItemFactory().randomItemId(ItemFilter::maxQLvl(mMonsterStats.level));
    }
}
