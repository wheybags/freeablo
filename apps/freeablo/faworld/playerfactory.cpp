#include "playerfactory.h"
#include "player.h"
#include "characterstats.h"
#include "itemmanager.h"

namespace FAWorld
{

PlayerFactory::PlayerFactory(DiabloExe::DiabloExe& exe)
    : mExe(exe)
{
}

Player* PlayerFactory::create(const std::string& playerClass)
{
    mPlayer = new Player();
    mCharStats = mExe.getCharacterStat(playerClass);

    if(playerClass == "Warrior")
    {
        createWarrior();
    }
    else if(playerClass == "Rogue")
    {
        createRogue();
    }
    else
    {
        createSorcerer();
    }

    mPlayer->setStats(mStats);
    mStats->setActor(mPlayer);
    mPlayer->mInventory.collectEffects();
    mStats->recalculateDerivedStats();

    return mPlayer;
}

void PlayerFactory::createWarrior()
{
    ItemManager & itemManager = ItemManager::get();

    mStats = new FAWorld::MeleeStats(mCharStats, mPlayer);

    FAWorld::Item item = itemManager.getBaseItem(125);
    mPlayer->mInventory.putItem(
                item,
                FAWorld::Item::eqLEFTHAND,
                FAWorld::Item::eqFLOOR,
                0, 0, 0, false);

    item = itemManager.getBaseItem(18);
    mPlayer->mInventory.putItem(
                item,
                FAWorld::Item::eqRIGHTHAND,
                FAWorld::Item::eqFLOOR,
                0, 0, 0, false);

    item = itemManager.getBaseItem(26);
    mPlayer->mInventory.putItem(
                item,
                FAWorld::Item::eqINV,
                FAWorld::Item::eqFLOOR,
                0, 0, 0, false);

    item = itemManager.getBaseItem(43);
    item.setCount(100);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqINV,
                               FAWorld::Item::eqFLOOR,
                               3, 0, 0, false);

    item = itemManager.getBaseItem(79);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(79);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 1, false);
    mPlayer->setSpriteClass("warrior");
    mPlayer->setIdleAnimation("plrgfx/warrior/wld/wldst.cl2");
    mPlayer->setWalkAnimation("plrgfx/warrior/wld/wldwl.cl2");
}

void PlayerFactory::createRogue()
{
    ItemManager & itemManager = ItemManager::get();

    mStats = new FAWorld::RangerStats(mCharStats, mPlayer);

    FAWorld::Item item = itemManager.getBaseItem(121);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqLEFTHAND,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(43);
    item.setCount(100);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqINV,
                               FAWorld::Item::eqFLOOR,
                               3, 0, 0,false);

    item = itemManager.getBaseItem(79);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(79);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 1, false);

    mPlayer->setSpriteClass("rogue");
    mPlayer->setIdleAnimation("plrgfx/rogue/rlb/rlbst.cl2");
    mPlayer->setWalkAnimation("plrgfx/rogue/rlb/rlbwl.cl2");

}

void PlayerFactory::createSorcerer()
{
    ItemManager & itemManager = ItemManager::get();

    mStats = new FAWorld::MageStats(mCharStats, mPlayer);

    FAWorld::Item item = itemManager.getBaseItem(124);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqLEFTHAND,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(43);
    item.setCount(100);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqINV,
                               FAWorld::Item::eqFLOOR,
                               3, 0, 0, false);

    item = itemManager.getBaseItem(81);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(81);
    mPlayer->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqBELT,
                               0, 0, 1, false);

    mPlayer->setSpriteClass("sorceror");
    mPlayer->setIdleAnimation("plrgfx/sorceror/slt/sltst.cl2");
    mPlayer->setWalkAnimation("plrgfx/sorceror/slt/sltwl.cl2");
}


}
