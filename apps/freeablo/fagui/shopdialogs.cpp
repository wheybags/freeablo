#include "shopdialogs.h"
#include "../engine/enginemain.h"
#include "../engine/localinputhandler.h"
#include "../faworld/player.h"
#include "../faworld/storedata.h"
#include "guimanager.h"
#include <faworld/item/equipmentitem.h>
#include <faworld/item/equipmentitembase.h>
#include <fmt/format.h>

namespace FAGui
{
    MessagePopup::MessagePopup(GuiManager& guiManager, const std::string& message) : CharacterDialoguePopup(guiManager, false), mMessage(message) {}

    CharacterDialoguePopup::DialogData MessagePopup::getDialogData()
    {
        DialogData retval;
        retval.introduction = {{this->mMessage, TextColor::golden, false}};
        retval.addMenuOption({{"OK", TextColor::white, true}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    ConfirmTransactionPopup::ConfirmTransactionPopup(GuiManager& guiManager, const MenuEntry& intro, const std::vector<MenuEntry>& desc, Transaction t)
        : CharacterDialoguePopup(guiManager, true), mTransaction(t), mIntroduction(intro), mDescription(desc)
    {
    }

    CharacterDialoguePopup::DialogData ConfirmTransactionPopup::getDialogData()
    {
        DialogData retval;
        std::string transaction = mTransaction == Transaction::buy ? "buy" : "sell";
        std::string message = "Are you sure you want to " + transaction + " this item?";
        retval.introduction = {mIntroduction};

        for (auto& desc : mDescription)
        {
            desc.clickable = false;
        }

        retval.addMenuOption(mDescription, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
        retval.addMenuOption({{}, {message, false}, {}}, []() { return CharacterDialoguePopup::UpdateResult::DoNothing; });
        retval.addMenuOption({{"Yes"}}, mAction);
        retval.addMenuOption({{"No"}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    void ConfirmTransactionPopup::addAction(std::function<CharacterDialoguePopup::UpdateResult()> act) { mAction = act; }

    ShopSellDialog::ShopSellDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::function<bool(const FAWorld::Item* item)> filter)
        : CharacterDialoguePopup(guiManager, true), mFilter(filter), mShopkeeper(shopkeeper)
    {
    }

    CharacterDialoguePopup::DialogData ShopSellDialog::getDialogData()
    {
        DialogData retval;

        auto& inventory = mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory;

        std::vector<FAWorld::EquipTarget> sellableItems;
        {
            auto addItem = [&](FAWorld::EquipTarget target) {
                const FAWorld::Item* item = inventory.getItemAt(target);
                if (item && mFilter(item) && item->getBase()->mType != ItemType::gold)
                    sellableItems.push_back(target);
            };

            for (const FAWorld::BasicInventoryBox& item :
                 mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.getInv(FAWorld::EquipTargetType::inventory))
                addItem(FAWorld::MakeEquipTarget<FAWorld::EquipTargetType::inventory>(item.topLeft.x, item.topLeft.y));
            for (const FAWorld::BasicInventoryBox& item :
                 mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.getInv(FAWorld::EquipTargetType::belt))
                addItem(FAWorld::MakeEquipTarget<FAWorld::EquipTargetType::belt>(item.topLeft.x));
        }

        int32_t totalGold = mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.getTotalGold();

        retval.introduction = {
            {fmt::format("{}            Your gold : {}", totalGold, (sellableItems.empty() ? "You have nothing I want." : "Which item is for sale?")),
             TextColor::golden,
             false}};

        const MenuEntry& intro = retval.introduction[0];

        for (FAWorld::EquipTarget& item : sellableItems)
        {
            std::vector<FAGui::MenuEntry> description = inventory.getItemAt(item)->descriptionForMerchants();
            ConfirmTransactionPopup::Transaction transaction = ConfirmTransactionPopup::Transaction::sell;
            ConfirmTransactionPopup* confirmPopup = new ConfirmTransactionPopup(this->mGuiManager, intro, description, transaction);
            retval.addMenuOption(description, [this, item, confirmPopup]() {
                this->sellItem(item, confirmPopup);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
        }

        retval.addMenuOption({{"Quit"}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    void ShopSellDialog::sellItem(const FAWorld::EquipTarget& item, ConfirmTransactionPopup* confirmPopup)
    {
        FAWorld::CharacterInventory& inventory = mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory;
        const FAWorld::Item* invItem = inventory.getItemAt(item);
        debug_assert(invItem);

        const int32_t price = invItem->getPrice();

        if (!mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory.canFitGold(price))
        {
            this->mGuiManager.mDialogManager.pushDialog(new MessagePopup(this->mGuiManager, "You do not have enough room in inventory"));
            return;
        }

        auto sellAction = [this, item]() {
            FAWorld::PlayerInput::SellItemData input{item, mShopkeeper.getId()};
            Engine::EngineMain::get()->getLocalInputHandler()->addInput(
                FAWorld::PlayerInput(input, Engine::EngineMain::get()->mWorld->getCurrentPlayer()->getId()));

            return CharacterDialoguePopup::UpdateResult::PopDialog;
        };

        confirmPopup->addAction(sellAction);
        this->mGuiManager.mDialogManager.pushDialog(confirmPopup);
    }

    ShopBuyDialog::ShopBuyDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::vector<FAWorld::StoreItem>& items)
        : CharacterDialoguePopup(guiManager, true), mItems(items), mShopkeeper(shopkeeper)
    {
    }

    CharacterDialoguePopup::DialogData ShopBuyDialog::getDialogData()
    {
        DialogData retval;

        FAWorld::Player* currPlayer = mGuiManager.mDialogManager.mWorld.getCurrentPlayer();
        FAWorld::CharacterInventory& inventory = currPlayer->mInventory;

        retval.introduction = {
            {fmt::format("{}           Your gold : {}", inventory.getTotalGold(), "I have these items for sale :"), TextColor::golden, false}};
        const auto& intro = retval.introduction[0];

        for (size_t i = 0; i < mItems.size(); i++)
        {
            FAWorld::StoreItem& item = mItems[i];
            std::vector<FAGui::MenuEntry> description = item.item->descriptionForMerchants();

            if (FAWorld::EquipmentItem* equipmentItem = item.item->getAsEquipmentItem())
            {
                if (!equipmentItem->getBase()->usableByPlayer(*currPlayer))
                {
                    for (auto& section : description)
                        section.textColor = TextColor::red;
                }
            }

            retval.addMenuOption(item.item->descriptionForMerchants(), [this, i, &item, &intro]() {
                ConfirmTransactionPopup::Transaction transaction = ConfirmTransactionPopup::Transaction::buy;
                ConfirmTransactionPopup* confirmPopup =
                    new ConfirmTransactionPopup(this->mGuiManager, intro, item.item->descriptionForMerchants(), transaction);
                this->buyItem(i, confirmPopup);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });
        }

        retval.addMenuOption({{"Quit"}}, []() { return CharacterDialoguePopup::UpdateResult::PopDialog; });

        return retval;
    }

    void ShopBuyDialog::buyItem(size_t index, ConfirmTransactionPopup* confirmPopup)
    {
        FAWorld::StoreItem& item = mItems[index];

        auto& inventory = mGuiManager.mDialogManager.mWorld.getCurrentPlayer()->mInventory;
        if (inventory.getTotalGold() < item.item->getPrice())
        {
            this->mGuiManager.mDialogManager.pushDialog(new MessagePopup(this->mGuiManager, "You do not have enough gold"));
            return;
        }

        if (!inventory.getInv(FAWorld::EquipTargetType::inventory).canFitItem(*item.item))
        {
            this->mGuiManager.mDialogManager.pushDialog(new MessagePopup(this->mGuiManager, "You do not have enough room in inventory"));
            return;
        }

        auto buyAction = [&]() {
            // send the buy action to the input handler
            auto input = FAWorld::PlayerInput::BuyItemData{item.storeId, mShopkeeper.getId()};
            Engine::EngineMain::get()->getLocalInputHandler()->addInput(
                FAWorld::PlayerInput(input, Engine::EngineMain::get()->mWorld->getCurrentPlayer()->getId()));

            return CharacterDialoguePopup::UpdateResult::PopDialog;
        };

        confirmPopup->addAction(buyAction);
        this->mGuiManager.mDialogManager.pushDialog(confirmPopup);
    }
}
