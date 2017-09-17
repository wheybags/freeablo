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
    auto charStats = mExe.getCharacterStat(playerClass);
    auto player = new Player(playerClass, charStats);

    if(playerClass == "Warrior")
        createWarrior(player);
    else if(playerClass == "Rogue")
        createRogue(player);
    else
        createSorcerer(player);

    player->getInventory ().collectEffects();

    return player;
}

void PlayerFactory::loadTestingKit (Player *player)
{
   ItemManager & itemManager = ItemManager::get();
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Buckler"), FAWorld::MakeEquipTarget<Item::eqINV> (1, 0));
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Short Bow"), FAWorld::MakeEquipTarget<Item::eqINV> (3, 0));
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Ring"), FAWorld::MakeEquipTarget<Item::eqINV> (1, 2));
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Ring"), FAWorld::MakeEquipTarget<Item::eqINV> (2, 2));
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Amulet"), FAWorld::MakeEquipTarget<Item::eqINV> (1, 3));
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Helm"), FAWorld::MakeEquipTarget<Item::eqINV> (5, 0));
   player->getInventory ().putItemUnsafe(itemManager.getItemByName("Rags"), FAWorld::MakeEquipTarget<Item::eqINV> (7, 0));
}

void PlayerFactory::createWarrior(Player* player) const
{
    ItemManager & itemManager = ItemManager::get();

    FAWorld::Item item = itemManager.getItemByName("Short Sword");
    player->getInventory ().putItemUnsafe(
        item, FAWorld::MakeEquipTarget<Item::eqLEFTHAND> ());

    item = itemManager.getItemByName("Buckler");
    player->getInventory ().putItemUnsafe(
        item, FAWorld::MakeEquipTarget<Item::eqRIGHTHAND> ());

    item = itemManager.getItemByName("Club");
    player->getInventory ().putItemUnsafe(
        item, FAWorld::MakeEquipTarget<Item::eqINV> (0, 0));

    item = itemManager.getItemByName("Gold");
    item.setCount(100);
    player->getInventory ().putItemUnsafe(
        item, FAWorld::MakeEquipTarget<Item::eqINV> (0, 3));

    for (auto i = 0; i < 2; ++i)
        {
            item = itemManager.getItemByName("Potion of Healing");
            player->getInventory ().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqBELT> (i));
        }

    player->setSpriteClass("warrior");
    player->getAnimationManager().setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldst.cl2"));
    player->getAnimationManager().setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldwl.cl2"));
    //loadTestingKit (player);
}

void PlayerFactory::createRogue(Player* player) const
{
    ItemManager & itemManager = ItemManager::get();

    FAWorld::Item item = itemManager.getItemByName("Short Bow");
    player->getInventory ().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqLEFTHAND> ());

    item = itemManager.getItemByName("Gold");
    item.setCount(100);
    player->getInventory ().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqINV> (0, 3));

    for (auto i = 0; i < 2; ++i)
        {
            item = itemManager.getItemByName("Potion of Healing");
            player->getInventory ().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqBELT> (i));
        }

    player->setSpriteClass("rogue");
    player->getAnimationManager().setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbst.cl2"));
    player->getAnimationManager().setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbwl.cl2"));
}

void PlayerFactory::createSorcerer(Player* player) const
{
    ItemManager & itemManager = ItemManager::get();

    FAWorld::Item item = itemManager.getItemByName("Short Staff of Charged Bolt");
    player->getInventory ().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqLEFTHAND> ());

    item = itemManager.getItemByName("Gold");
    item.setCount(100);
    player->getInventory ().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqINV> (0, 3));

    for (auto i = 0; i < 2; ++i)
        {
            item = itemManager.getItemByName("Potion of Mana");
            player->getInventory().putItemUnsafe(item, FAWorld::MakeEquipTarget<Item::eqBELT> (i));
        }

    player->setSpriteClass("sorceror");
    player->getAnimationManager().setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltst.cl2"));
    player->getAnimationManager().setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltwl.cl2"));

}


}
