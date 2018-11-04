#include "shopdialogs.h"

#include "../engine/enginemain.h"
#include "../engine/localinputhandler.h"
#include "../faworld/item.h"
#include "../faworld/player.h"
#include "../faworld/storedata.h"
#include "guimanager.h"

namespace FAGui
{
    MessagePopup::MessagePopup(GuiManager& guiManager, const std::string& message) : CharacterDialoguePopup(guiManager, false), mMessage(message) {}

    CharacterDialoguePopup::DialogData MessagePopup::getDialogData()
    {
        DialogData retval;
        retval.introduction = {this->mMessage};
        retval.addMenuOption({"OK"}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    ShopSellDialog::ShopSellDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::function<bool(const FAWorld::Item& item)> filter)
        : CharacterDialoguePopup(guiManager, true), mFilter(filter), mShopkeeper(shopkeeper)
    {
    }

    CharacterDialoguePopup::DialogData ShopSellDialog::getDialogData()
    {
        DialogData retval;

        auto& inventory = mGuiManager.mPlayer->mInventory;

        std::vector<FAWorld::EquipTarget> sellableItems;
        {
            auto addItem = [&](FAWorld::EquipTarget target) {
                const FAWorld::Item& item = inventory.getItemAt(target);
                if (!item.isEmpty() && item.mIsReal && mFilter(item) && item.getType() != FAWorld::ItemType::gold)
                    sellableItems.push_back(target);
            };

            for (const FAWorld::Item& item : mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.getInv(FAWorld::EquipTargetType::inventory))
                addItem(FAWorld::MakeEquipTarget<FAWorld::EquipTargetType::inventory>(item.mInvX, item.mInvY));
            for (const FAWorld::Item& item : mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.getInv(FAWorld::EquipTargetType::belt))
                addItem(FAWorld::MakeEquipTarget<FAWorld::EquipTargetType::belt>(item.mInvX));
        }

        int32_t totalGold = mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.getTotalGold();

        retval.introduction = {
            (boost::format("%2%            Your gold : %1%") % totalGold % (sellableItems.empty() ? "You have nothing I want." : "Which item is for sale?"))
                .str()};

        for (FAWorld::EquipTarget item : sellableItems)
        {
            retval.addMenuOption(inventory.getItemAt(item).descriptionForMerchants(), [this, item]() {
                this->sellItem(item);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
        }

        retval.addMenuOption({"Quit"}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    void ShopSellDialog::sellItem(FAWorld::EquipTarget item)
    {
        auto input = FAWorld::PlayerInput::SellItemData{item, mShopkeeper.getId()};
        Engine::EngineMain::get()->getLocalInputHandler()->addInput(
            FAWorld::PlayerInput(input, Engine::EngineMain::get()->mWorld->getCurrentPlayer()->getId()));
    }

    ShopBuyDialog::ShopBuyDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::vector<FAWorld::StoreItem>& items)
        : CharacterDialoguePopup(guiManager, true), mItems(items), mShopkeeper(shopkeeper)
    {
    }

    CharacterDialoguePopup::DialogData ShopBuyDialog::getDialogData()
    {
        DialogData retval;

        auto& inventory = mGuiManager.mPlayer->mInventory;
        retval.introduction = {(boost::format("%2%           Your gold : %1%") % inventory.getTotalGold() % "I have these items for sale :").str()};

        for (size_t i = 0; i < mItems.size(); i++)
        {
            FAWorld::StoreItem& item = mItems[i];
            retval.addMenuOption(item.item.descriptionForMerchants(), [this, i]() {
                this->buyItem(i);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
        }

        retval.addMenuOption({"Quit"}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    void ShopBuyDialog::buyItem(size_t index)
    {
        FAWorld::StoreItem& item = mItems[index];

        auto& inventory = mGuiManager.mPlayer->mInventory;
        if (inventory.getTotalGold() < item.item.getPrice())
        {
            this->mGuiManager.mDialogManager.mDialogStack.emplace_back(new MessagePopup(this->mGuiManager, "You do not have enough gold"));
            return;
        }

        if (!inventory.getInv(FAWorld::EquipTargetType::inventory).canFitItem(item.item))
        {
            this->mGuiManager.mDialogManager.mDialogStack.emplace_back(new MessagePopup(this->mGuiManager, "You do not have enough room in inventory"));
            return;
        }

        // send the buy action to the input handler
        auto input = FAWorld::PlayerInput::BuyItemData{item.storeId, mShopkeeper.getId()};
        Engine::EngineMain::get()->getLocalInputHandler()->addInput(
            FAWorld::PlayerInput(input, Engine::EngineMain::get()->mWorld->getCurrentPlayer()->getId()));
    }
}
