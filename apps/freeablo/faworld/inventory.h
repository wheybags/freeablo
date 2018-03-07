#pragma once
#include "item.h"
#include <boost/signals2/signal.hpp>
#include <misc/array2d.h>
#include <misc/misc.h>
#include <stdint.h>

namespace FAWorld
{
    class EquipTarget;
    class Actor;
    struct ExchangeResult;
    class ItemFactory;

    class Inventory
    {
    public:
        enum class PlacementCheckOrder
        {
            fromLeftBottom,  // for 1x1 items
            fromLeftTop,     // for 1x3, 2x3 items, returning shield
            fromRightBottom, // for new piles of gold
            specialFor1x2,
            specialFor2x2,
        };

    private:
        using self = Inventory;

    public:
        Inventory();

        void save(FASaveGame::GameSaver& saver);
        void load(FASaveGame::GameLoader& loader, const DiabloExe::DiabloExe& exe);

        bool isValidCell(int x, int y) const { return x >= 0 && x < mInventoryBox.width() && y >= 0 && y < mInventoryBox.height(); }

        const Item& getItemAt(const EquipTarget& target) const;
        Item& getItemAt(const EquipTarget& target);
        void itemSlotLeftMouseButtonDown(EquipTarget target);
        void beltMouseLeftButtonDown(double x);
        void inventoryMouseLeftButtonDown(Misc::Point cell);
        void setCursorHeld(const Item& item);
        // this function uses no checks for placing item, may lead to erroneous result
        // in general there's no need for safe function because items are placed either through exchange with cursor
        // or autoplacement. Currently it used externally only for placing starting items.
        void putItemUnsafe(const Item& item, const EquipTarget& target);

        bool fillExistingGoldItems(Item& goldItem);
        bool autoPlaceItem(Item& item, boost::optional<PlacementCheckOrder> override_order = boost::none);

        // if we ever need write access to these - just ditch the getters and make the vars public
        const Item& getBody() const { return mBody; }
        const Item& getLeftHand() const { return mLeftHand; }
        const Item& getRightHand() const { return mRightHand; }
        const Misc::Array2D<Item>& getInventoryBox() const { return mInventoryBox; }
        const std::vector<Item>& getBelt() const { return mBelt; }
        std::vector<EquipTarget> getBeltAndInventoryItemPositions() const;
        Item takeOut(const EquipTarget& target);
        // note: this function could not just call autoPlaceItem because quantity may include more than 5000 pieces
        void placeGold(int quantity, const ItemFactory& itemFactory);
        ItemBonus getTotalItemBonus() const;
        void takeOutGold(int32_t quantity);
        bool tryPlace(const Item& item, int32_t x, int32_t y);
        bool couldBePlacedToInventory(const Item& item) const;

    private:
        void updateCursor();
        bool checkStatsRequirement(const Item& item) const;
        bool couldBePlacedToInventory(const Item& item, const EquipTarget& target) const;
        auto needsToBeExchanged(const Item& item, const EquipTarget& target) const -> ExchangeResult;
        EquipTarget avoidLinks(const EquipTarget& target);
        void layItem(const Item& item, int32_t x, int32_t y);
        bool exchangeWithCursor(EquipTarget takeoutTarget, boost::optional<EquipTarget> maybePlacementTarget);
        bool exchangeWithCursor(EquipTarget takeoutTarget);
        // tries to place item so it top left corner is on x, y. returns false if it's impossible.
        bool couldBePlacedToInventory(const Item& item, int32_t x, int32_t y) const;

    public:
        // This is not serialised - it should be reconnected by other means
        boost::signals2::signal<void()> equipChanged;

    private:
        static constexpr int32_t inventoryWidth = 10;
        static constexpr int32_t inventoryHeight = 4;
        static constexpr int32_t beltWidth = 8;

        Misc::Array2D<Item> mInventoryBox = Misc::Array2D<Item>(inventoryWidth, inventoryHeight);
        std::vector<Item> mBelt = std::vector<Item>(beltWidth);
        Item mHead;
        Item mBody;
        Item mLeftRing;
        Item mRightRing;
        Item mAmulet;
        Item mLeftHand;
        Item mRightHand;
        Item mCursorHeld;
    };
}
