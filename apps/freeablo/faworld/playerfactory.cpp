#include "playerfactory.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemfactory.h"
#include "player.h"

namespace FAWorld
{

    PlayerFactory::PlayerFactory(const DiabloExe::DiabloExe& exe, const ItemFactory& itemFactory) : mExe(exe), mItemFactory(itemFactory) {}

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
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::buckler), MakeEquipTarget<EquipTargetType::inventory>(1, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseShortBow), MakeEquipTarget<EquipTargetType::inventory>(3, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseRingQlvl5), MakeEquipTarget<EquipTargetType::inventory>(1, 2));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseRingQlvl5), MakeEquipTarget<EquipTargetType::inventory>(2, 2));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseAmuletQlvl8), MakeEquipTarget<EquipTargetType::inventory>(1, 3));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseHelm), MakeEquipTarget<EquipTargetType::inventory>(5, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseRags), MakeEquipTarget<EquipTargetType::inventory>(7, 0));
    }

    void PlayerFactory::createWarrior(Player* player) const
    {
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortSword), MakeEquipTarget<EquipTargetType::leftHand>());
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::buckler, BaseItemGenOptions().setRandomizeArmorClass(false)),
                                         MakeEquipTarget<EquipTargetType::rightHand>());
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::club), MakeEquipTarget<EquipTargetType::inventory>(0, 0));
        {
            auto item = mItemFactory.generateBaseItem(ItemId::gold);
            item.setCount(100);
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::inventory>(0, 3));
        }

        for (auto i = 0; i < 2; ++i)
        {
            player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::potionOfHealing), MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setSpriteClass("warrior");
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldwl.cl2"));
    }

    void PlayerFactory::createRogue(Player* player) const
    {
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortBow), MakeEquipTarget<EquipTargetType::leftHand>());
        {
            auto item = mItemFactory.generateBaseItem(ItemId::gold);
            item.setCount(100);
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::inventory>(0, 3));
        }

        for (auto i = 0; i < 2; ++i)
        {
            player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::potionOfHealing), MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setSpriteClass("rogue");
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbwl.cl2"));
    }

    void PlayerFactory::createSorcerer(Player* player) const
    {
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortStaffOfChargedBolt), MakeEquipTarget<EquipTargetType::leftHand>());
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortBow), MakeEquipTarget<EquipTargetType::leftHand>());
        {
            auto item = mItemFactory.generateBaseItem(ItemId::gold);
            item.setCount(100);
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::inventory>(0, 3));
        }

        for (auto i = 0; i < 2; ++i)
        {
            player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::potionOfMana), MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setSpriteClass("sorceror");
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltwl.cl2"));
    }
}
