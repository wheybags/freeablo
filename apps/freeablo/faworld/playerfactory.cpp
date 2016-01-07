#include "playerfactory.h"
#include "player.h"
#include "characterstats.h"
#include "itemmanager.h"

namespace FAWorld
{

PlayerFactory::PlayerFactory(const DiabloExe::DiabloExe& exe)
    : mExe(exe)
{
}

Player* PlayerFactory::create(const std::string& playerClass) const
{
    auto player = new Player();
    auto charStats = mExe.getCharacterStat(playerClass);

    ActorStats* stats = NULL;

    if(playerClass == "Warrior")
        stats = createWarrior(player, charStats);
    else if(playerClass == "Rogue")
        stats = createRogue(player, charStats);
    else
        stats = createSorcerer(player, charStats);

    player->setStats(stats);
    stats->setActor(player);

    player->mInventory.collectEffects();
    stats->recalculateDerivedStats();

    return player;
}

ActorStats* PlayerFactory::createWarrior(Player* player, const DiabloExe::CharacterStats& charStats) const
{
    ItemManager & itemManager = ItemManager::get();

    auto stats = new FAWorld::MeleeStats(charStats, player);

    FAWorld::Item item = itemManager.getBaseItem(125);
    player->mInventory.putItem(
                item,
                FAWorld::Item::eqLEFTHAND,
                FAWorld::Item::eqFLOOR,
                0, 0, 0, false);

    item = itemManager.getBaseItem(18);
    player->mInventory.putItem(
                item,
                FAWorld::Item::eqRIGHTHAND,
                FAWorld::Item::eqFLOOR,
                0, 0, 0, false);

    item = itemManager.getBaseItem(26);
    player->mInventory.putItem(
                item,
                FAWorld::Item::eqINV,
                FAWorld::Item::eqFLOOR,
                0, 0, 0, false);

    item = itemManager.getBaseItem(43);
    item.setCount(100);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqINV,
                               FAWorld::Item::eqFLOOR,
                               3, 0, 0, false);

    item = itemManager.getBaseItem(79);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(79);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 1, false);
    player->setSpriteClass("warrior");
    player->setIdleAnimation("plrgfx/warrior/wld/wldst.cl2");
    player->setWalkAnimation("plrgfx/warrior/wld/wldwl.cl2");

    return stats;
}

ActorStats* PlayerFactory::createRogue(Player* player, const DiabloExe::CharacterStats& charStats) const
{
    ItemManager & itemManager = ItemManager::get();

    auto stats = new FAWorld::RangerStats(charStats, player);

    FAWorld::Item item = itemManager.getBaseItem(121);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqLEFTHAND,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(43);
    item.setCount(100);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqINV,
                               FAWorld::Item::eqFLOOR,
                               3, 0, 0,false);

    item = itemManager.getBaseItem(79);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(79);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 1, false);

    player->setSpriteClass("rogue");
    player->setIdleAnimation("plrgfx/rogue/rlb/rlbst.cl2");
    player->setWalkAnimation("plrgfx/rogue/rlb/rlbwl.cl2");

    return stats;
}

ActorStats* PlayerFactory::createSorcerer(Player* player, const DiabloExe::CharacterStats& charStats) const
{
    ItemManager & itemManager = ItemManager::get();

    auto stats = new FAWorld::MageStats(charStats, player);

    FAWorld::Item item = itemManager.getBaseItem(124);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqLEFTHAND,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(43);
    item.setCount(100);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqINV,
                               FAWorld::Item::eqFLOOR,
                               3, 0, 0, false);

    item = itemManager.getBaseItem(81);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqFLOOR,
                               0, 0, 0, false);

    item = itemManager.getBaseItem(81);
    player->mInventory.putItem(item,
                               FAWorld::Item::eqBELT,
                               FAWorld::Item::eqBELT,
                               0, 0, 1, false);

    player->setSpriteClass("sorceror");
    player->setIdleAnimation("plrgfx/sorceror/slt/sltst.cl2");
    player->setWalkAnimation("plrgfx/sorceror/slt/sltwl.cl2");

    return stats;
}


}
