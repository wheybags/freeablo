#include "playerfactory.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemfactory.h"
#include "player.h"

namespace FAWorld
{

    PlayerFactory::PlayerFactory(const DiabloExe::DiabloExe& exe, const ItemFactory& itemFactory) : mExe(exe), mItemFactory(itemFactory) {}

    Player* PlayerFactory::create(World& world, const std::string& playerClass) const
    {
        auto charStats = mExe.getCharacterStat(playerClass);
        auto player = new Player(world, charStats);

        if (playerClass == "Warrior")
            createWarrior(player);
        else if (playerClass == "Rogue")
            createRogue(player);
        else
            createSorcerer(player);

        player->mPlayerInitialised = true;

        return player;
    }

    void PlayerFactory::loadTestingKit(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::buckler));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::shortBow));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::baseRingQlvl5));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::baseRingQlvl5));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::baseAmuletQlvl8));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::baseHelm));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::baseRags));
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::baseDagger));
    }

    void PlayerFactory::fillWithGold(Player* player) const
    {
        // function for testing
        const BasicInventory& inv = player->mInventory.getInv(EquipTargetType::inventory);

        bool hasSlots = true;
        while (hasSlots)
        {
            player->mInventory.placeGold(1000, mItemFactory);

            hasSlots = false;
            for (const Item& slot : inv)
                if (slot.isEmpty())
                    hasSlots = true;
        }
    }

    void PlayerFactory::fillWithPotions(Player* player) const
    {
        // function for testing
        const BasicInventory& inv = player->mInventory.getInv(EquipTargetType::inventory);

        bool hasSlots = true;
        while (hasSlots)
        {
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));

            hasSlots = false;
            for (const Item& slot : inv)
                if (slot.isEmpty())
                    hasSlots = true;
        }
    }

    void PlayerFactory::createWarrior(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::shortSword));
        player->mInventory.forcePlaceItem(mItemFactory.generateBaseItem(ItemId::buckler), MakeEquipTarget<EquipTargetType::rightHand>());
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::club));
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));

        player->setPlayerClass(PlayerClass::warrior);
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/warrior/wld/wldwl.cl2"));
        // loadTestingKit (player);
        // fillWithGold(player);
    }

    void PlayerFactory::createRogue(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::shortBow));
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));

        player->setPlayerClass(PlayerClass::rogue);
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/rogue/rlb/rlbwl.cl2"));
    }

    void PlayerFactory::createSorcerer(Player* player) const
    {
        {
            auto item = mItemFactory.generateBaseItem(ItemId::shortStaffOfChargedBolt);
            item.mMaxCharges = item.mCurrentCharges = 40;
            player->mInventory.autoPlaceItem(item);
        }
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));

        player->setPlayerClass(PlayerClass::sorcerer);
        player->mAnimation.setAnimation(AnimState::idle, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltst.cl2"));
        player->mAnimation.setAnimation(AnimState::walk, FARender::Renderer::get()->loadImage("plrgfx/sorceror/slt/sltwl.cl2"));
    }
}
