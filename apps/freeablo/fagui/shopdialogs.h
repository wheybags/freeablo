#pragma once
#include "characterdialoguepopup.h"

namespace FAWorld
{
    class Item;
    struct StoreItem;
}

namespace FAGui
{
    class MessagePopup : public CharacterDialoguePopup
    {
    public:
        MessagePopup(GuiManager& guiManager, const std::string& message);

    protected:
        virtual DialogData getDialogData() override;

    private:
        std::string mMessage;
    };

    class ConfirmTransactionPopup : public CharacterDialoguePopup
    {
    public:
        enum class Transaction
        {
            buy,
            sell
        };

        ConfirmTransactionPopup(GuiManager& guiManager, const MenuEntry& intro, const std::vector<MenuEntry>& desc, Transaction t);
        void addAction(std::function<CharacterDialoguePopup::UpdateResult()> action);

    protected:
        virtual DialogData getDialogData() override;

    private:
        Transaction mTransaction;
        MenuEntry mIntroduction;
        std::vector<MenuEntry> mDescription;
        std::function<CharacterDialoguePopup::UpdateResult()> mAction;
    };

    class ShopSellDialog : public CharacterDialoguePopup
    {
    public:
        ShopSellDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::function<bool(const FAWorld::Item& item)> filter);

    protected:
        virtual DialogData getDialogData() override;

    private:
        void sellItem(const FAWorld::EquipTarget& item, ConfirmTransactionPopup* confirmPopup);

    private:
        std::function<bool(const FAWorld::Item& item)> mFilter;
        const FAWorld::Actor& mShopkeeper;
    };

    class ShopBuyDialog : public CharacterDialoguePopup
    {
    public:
        ShopBuyDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::vector<FAWorld::StoreItem>& items);

    protected:
        virtual DialogData getDialogData() override;

    private:
        void buyItem(size_t index, ConfirmTransactionPopup* confirmPopup);

    private:
        std::vector<FAWorld::StoreItem>& mItems;
        const FAWorld::Actor& mShopkeeper;
    };
}
