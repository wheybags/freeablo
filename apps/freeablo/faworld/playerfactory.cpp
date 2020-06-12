#include "playerfactory.h"
#include "diabloexe/characterstats.h"
#include "equiptarget.h"
#include "item/equipmentitem.h"
#include "item/itemprefixorsuffix.h"
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

    void PlayerFactory::fillWithGold(Player* player) const
    {
        // function for testing
        const BasicInventory& inv = player->mInventory.getInv(EquipTargetType::inventory);

        bool hasSlots = true;
        while (hasSlots)
        {
            player->mInventory.placeGold(1000, mItemFactory);

            hasSlots = false;
            for (const BasicInventoryBox& slot : inv)
                if (!slot.item)
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
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("potion_of_healing"));

            hasSlots = false;
            for (const BasicInventoryBox& slot : inv)
                if (!slot.item)
                    hasSlots = true;
        }
    }

    void PlayerFactory::addWarriorItems(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("short_sword"));
        std::unique_ptr<Item> buckler = mItemFactory.generateBaseItem("buckler");
        player->mInventory.forcePlaceItem(buckler, MakeEquipTarget<EquipTargetType::rightHand>());
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("club"));
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("potion_of_healing"));
    }

    void PlayerFactory::addRogueItems(Player* player) const
    {
        player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("short_bow"));
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("potion_of_healing"));
    }

    void PlayerFactory::addSorcerorItems(Player* player) const
    {
        {
            auto item = mItemFactory.generateBaseItem("short_staff_of_charged_bolt");
            // item.mMaxCharges = item.mCurrentCharges = 40;
            player->mInventory.autoPlaceItem(item);
        }
        player->mInventory.placeGold(100, mItemFactory);

        for (int32_t i = 0; i < 2; ++i)
            player->mInventory.autoPlaceItem(mItemFactory.generateBaseItem("potion_of_healing"));
    }
}
