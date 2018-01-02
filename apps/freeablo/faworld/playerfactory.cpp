#include "playerfactory.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemmanager.h"
#include "player.h"

namespace FAWorld
{

    PlayerFactory::PlayerFactory(const DiabloExe::DiabloExe& exe) : mExe(exe) {}

    Player* PlayerFactory::create(const std::string& playerClass) const
    {
        auto charStats = mExe.getCharacterStat(playerClass);
        auto player = new Player(playerClass, charStats);

        if (playerClass == "Warrior")
            createWarrior(player);
        else if (playerClass == "Rogue")
            createRogue(player);
        else
            createSorcerer(player);

        return player;
    }

    void PlayerFactory::loadTestingKit(Player* player)
    {
        ItemManager& itemManager = ItemManager::get();
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Buckler"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(1, 0));
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Short Bow"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(3, 0));
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Ring"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(1, 2));
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Ring"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(2, 2));
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Amulet"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(1, 3));
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Helm"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(5, 0));
        player->mInventory.putItemUnsafe(itemManager.getItemByName("Rags"), FAWorld::MakeEquipTarget<EquipTargetType::inventory>(7, 0));
    }

    void PlayerFactory::createWarrior(Player* player) const
    {
        ItemManager& itemManager = ItemManager::get();

        FAWorld::Item item = itemManager.getItemByName("Short Sword");
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::leftHand>());

        item = itemManager.getItemByName("Buckler");
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::rightHand>());

        item = itemManager.getItemByName("Club");
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::inventory>(0, 0));

        item = itemManager.getItemByName("Gold");
        item.mCount = 100;
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::inventory>(0, 3));

        for (auto i = 0; i < 2; ++i)
        {
            item = itemManager.getItemByName("Potion of Healing");
            player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setSpriteClass("warrior");
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldwl.cl2"));
    }

    void PlayerFactory::createRogue(Player* player) const
    {
        ItemManager& itemManager = ItemManager::get();

        FAWorld::Item item = itemManager.getItemByName("Short Bow");
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::leftHand>());

        item = itemManager.getItemByName("Gold");
        item.mCount = 100;
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::inventory>(0, 3));

        for (auto i = 0; i < 2; ++i)
        {
            item = itemManager.getItemByName("Potion of Healing");
            player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setSpriteClass("rogue");
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbwl.cl2"));
    }

    void PlayerFactory::createSorcerer(Player* player) const
    {
        ItemManager& itemManager = ItemManager::get();

        FAWorld::Item item = itemManager.getItemByName("Short Staff of Charged Bolt");
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::leftHand>());

        item = itemManager.getItemByName("Gold");
        item.mCount = 100;
        player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::inventory>(0, 3));

        for (auto i = 0; i < 2; ++i)
        {
            item = itemManager.getItemByName("Potion of Mana");
            player->mInventory.putItemUnsafe(item, FAWorld::MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setSpriteClass("sorceror");
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltwl.cl2"));
    }
}
