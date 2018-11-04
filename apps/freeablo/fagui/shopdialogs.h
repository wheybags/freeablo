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

    class ShopSellDialog : public CharacterDialoguePopup
    {
    public:
        ShopSellDialog(GuiManager& guiManager, const FAWorld::Actor& shopkeeper, std::function<bool(const FAWorld::Item& item)> filter);

    protected:
        virtual DialogData getDialogData() override;

    private:
        void sellItem(FAWorld::EquipTarget item);

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
        void buyItem(size_t index);

    private:
        std::vector<FAWorld::StoreItem>& mItems;
        const FAWorld::Actor& mShopkeeper;
    };
}
