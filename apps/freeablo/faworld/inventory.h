
#pragma once

#include "item.h"
#include <boost/signals2/signal.hpp>
#include <diabloexe/diabloexe.h>
#include <misc/array2d.h>
#include <stdint.h>

namespace FAWorld
{
    struct EquipTarget;
    class Actor;
    struct ExchangeResult;

    class Inventory
    {
        enum class xorder
        {
            fromLeft,
            fromRight,
        };
        enum class yorder
        {
            fromTop,
            fromBottom
        };
        using self = Inventory;

    public:
        Inventory();

        void save(FASaveGame::GameSaver& saver);
        void load(FASaveGame::GameLoader& loader);

        bool isValidCell(int x, int y) { return x >= 0 && x < mInventoryBox.width() && y >= 0 && y < mInventoryBox.height(); }

        void dump();

        const Item& getItemAt(const EquipTarget& target) const;
        Item& getItemAt(const EquipTarget& target);
        std::vector<std::tuple<ItemEffectType, uint32_t, uint32_t, uint32_t>>& getTotalEffects();
        void itemSlotLeftMouseButtonDown(EquipTarget target);
        void beltMouseLeftButtonDown(double x);
        void inventoryMouseLeftButtonDown(double x, double y);
        void setCursorHeld(const Item& item);
        // this function uses no checks for placing item, may lead to erroneous result
        // in general there's no need for safe function because items are placed either through exchange with cursor
        // or autoplacement. Currently it used externally only for placing starting items.
        void putItemUnsafe(const Item& item, const EquipTarget& target);

        bool autoPlaceItem(const Item& item, boost::optional<std::pair<xorder, yorder>> override_order = boost::none);

        // if we ever need write access to these - just ditch the getters and make the vars public
        const Item& getBody() const { return mBody; }
        const Item& getLeftHand() const { return mLeftHand; }
        const Item& getRightHand() const { return mRightHand; }
        const Misc::Array2D<Item>& getInventoryBox() const { return mInventoryBox; }
        const std::vector<Item>& getBelt() const { return mBelt; }

    private:
        void updateCursor();
        bool checkStatsRequirement(const Item& item) const;
        bool isFit(const Item& item, const EquipTarget& target) const;
        auto needsToBeExchanged(const Item& item, const EquipTarget& target) const -> ExchangeResult;
        EquipTarget avoidLinks(const EquipTarget& target);
        Item takeOut(const EquipTarget& target);
        void layItem(const Item& item, int32_t x, int32_t y);
        bool exchangeWithCursor(EquipTarget takeoutTarget, boost::optional<EquipTarget> maybePlacementTarget);
        bool exchangeWithCursor(EquipTarget takeoutTarget);
        bool fitsAt(Item item, uint8_t x, uint8_t y);

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
