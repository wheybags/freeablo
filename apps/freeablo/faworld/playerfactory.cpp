#include "playerfactory.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemfactory.h"
#include "player.h"

namespace FAWorld
{

    PlayerFactory::PlayerFactory(const DiabloExe::DiabloExe& exe, const ItemFactory& itemFactory) : mExe(exe), mItemFactory(itemFactory) {}

    Player* PlayerFactory::create(World& world, PlayerClass playerClass) const
    {
        auto charStats = mExe.getCharacterStat(playerClassToString(playerClass));

        auto player = new Player(world, playerClass, charStats);

        switch (playerClass)
        {
            case PlayerClass::warrior:
                addWarriorItems(player);
                break;
            case PlayerClass::rogue:
                addRogueItems(player);
                break;
            case PlayerClass::sorceror:
                addSorcerorItems(player);
                break;
            case PlayerClass::none:
                invalid_enum(PlayerClass, playerClass);
        }

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

    void PlayerFactory::addWarriorItems(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::shortSword));
        player->mInventory.forcePlaceItem(mItemFactory.generateBaseItem(ItemId::buckler), MakeEquipTarget<EquipTargetType::rightHand>());
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::club));
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));
    }

    void PlayerFactory::addRogueItems(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::shortBow));
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));
    }

    void PlayerFactory::addSorcerorItems(Player* player) const
    {
        {
            auto item = mItemFactory.generateBaseItem(ItemId::shortStaffOfChargedBolt);
            item.mMaxCharges = item.mCurrentCharges = 40;
            player->mInventory.autoPlaceItem(item);
        }
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem(ItemId::potionOfHealing));
    }
}
