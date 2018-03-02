#include "playerfactory.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemfactory.h"
#include "player.h"
#include <boost/range/irange.hpp>

namespace FAWorld
{

    PlayerFactory::PlayerFactory(const DiabloExe::DiabloExe& exe, const ItemFactory& itemFactory) : mExe(exe), mItemFactory(itemFactory) {}

    Player* PlayerFactory::create(World& world, const std::string& playerClass) const
    {
        auto charStats = mExe.getCharacterStat(playerClass);
        auto player = new Player(world, playerClass, charStats);

        if (playerClass == "Warrior")
            createWarrior(player);
        else if (playerClass == "Rogue")
            createRogue(player);
        else
            createSorcerer(player);

        return player;
    }

    void PlayerFactory::loadTestingKit(Player* player) const
    {
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::buckler), MakeEquipTarget<EquipTargetType::inventory>(1, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortBow), MakeEquipTarget<EquipTargetType::inventory>(3, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseRingQlvl5), MakeEquipTarget<EquipTargetType::inventory>(1, 2));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseRingQlvl5), MakeEquipTarget<EquipTargetType::inventory>(2, 2));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseAmuletQlvl8), MakeEquipTarget<EquipTargetType::inventory>(1, 3));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseHelm), MakeEquipTarget<EquipTargetType::inventory>(5, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseRags), MakeEquipTarget<EquipTargetType::inventory>(7, 0));
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::baseDagger), MakeEquipTarget<EquipTargetType::inventory>(9, 0));
    }

    void PlayerFactory::fillWithGold(Player* player) const
    {
        // function for testing
        auto& box = player->mInventory.getInventoryBox();
        for (auto i : boost::irange(0, box.width()))
            for (auto j : boost::irange(0, box.height()))
            {
                auto target = MakeEquipTarget<EquipTargetType::inventory>(i, j);
                if (player->mInventory.getItemAt(target).isEmpty())
                {
                    auto g = mItemFactory.generateBaseItem(ItemId::gold);
                    g.mCount = 1000;
                    player->mInventory.putItemUnsafe(g, target);
                }
            }
    }

    void PlayerFactory::createWarrior(Player* player) const
    {
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortSword), MakeEquipTarget<EquipTargetType::leftHand>());
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::buckler, BaseItemGenOptions()), MakeEquipTarget<EquipTargetType::rightHand>());
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::club), MakeEquipTarget<EquipTargetType::inventory>(0, 0));
        {
            auto item = mItemFactory.generateBaseItem(ItemId::gold);
            item.mCount = 100;
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::inventory>(0, 3));
        }

        for (auto i = 0; i < 2; ++i)
        {
            player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::potionOfHealing), MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setPlayerClass(PlayerClass::warrior);
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldwl.cl2"));
        // loadTestingKit (player);
        // fillWithGold(player);
    }

    void PlayerFactory::createRogue(Player* player) const
    {
        player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::shortBow), MakeEquipTarget<EquipTargetType::leftHand>());
        {
            auto item = mItemFactory.generateBaseItem(ItemId::gold);
            item.mCount = 100;
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::inventory>(0, 3));
        }

        for (auto i = 0; i < 2; ++i)
        {
            player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::potionOfHealing), MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setPlayerClass(PlayerClass::rogue);
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbwl.cl2"));
    }

    void PlayerFactory::createSorcerer(Player* player) const
    {
        {
            auto item = mItemFactory.generateBaseItem(ItemId::shortStaffOfChargedBolt);
            item.mMaxCharges = item.mCurrentCharges = 40;
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::leftHand>());
        }
        {
            auto item = mItemFactory.generateBaseItem(ItemId::gold);
            item.mCount = 100;
            player->mInventory.putItemUnsafe(std::move(item), MakeEquipTarget<EquipTargetType::inventory>(0, 3));
        }

        for (auto i = 0; i < 2; ++i)
        {
            player->mInventory.putItemUnsafe(mItemFactory.generateBaseItem(ItemId::potionOfMana), MakeEquipTarget<EquipTargetType::belt>(i));
        }

        player->setPlayerClass(PlayerClass::sorcerer);
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltwl.cl2"));
    }
}
