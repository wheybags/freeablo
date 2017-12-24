
#pragma once

#include "item.h"
#include <diabloexe/diabloexe.h>
#include <map>
#include <set>
#include <stdint.h>

#include <boost/signals2/signal.hpp>

namespace FAWorld
{
    struct EquipTarget;
    class Player;
    class CharacterStatsBase;
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
        Inventory(Player* actor);
        static constexpr auto inventoryWidth = 10;
        static constexpr auto inventoryHeight = 4;
        static constexpr auto beltWidth = 8;

        static inline bool isValidCell(int x, int y) { return x >= 0 && x < inventoryWidth && y >= 0 && y < inventoryHeight; }

        void dump();

        void collectEffects();
        const Item& getItemAt(const EquipTarget& target) const;
        Item& getItemAt(const EquipTarget& target);
        uint32_t getTotalAttackDamage();
        uint32_t getTotalArmourClass();
        std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>>& getTotalEffects();
        void itemSlotLeftMouseButtonDown(EquipTarget target);
        void beltMouseLeftButtonDown(double x);
        void inventoryMouseLeftButtonDown(double x, double y);
        void setCursorHeld(const Item& item);
        // this function uses no checks for placing item, may lead to erroneous result
        // in general there's no need for safe function because items are placed either through exchange with cursor
        // or autoplacement. Currently it used externally only for placing starting items.
        void putItemUnsafe(const Item& item, const EquipTarget& target);

        boost::signals2::signal<void()> equipChanged;

    private:
        void updateCursor();

    private:
        Item mInventoryBox[inventoryHeight][inventoryWidth];
        Item mBelt[beltWidth];
        Item mHead;
        Item mBody;
        Item mLeftRing;
        Item mRightRing;
        Item mAmulet;
        Item mLeftHand;
        Item mRightHand;
        Item mCursorHeld;
        uint32_t mArmourClassTotal;
        uint32_t mAttackDamageTotal;
        Player* mActor;
        std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> mItemEffects;
        bool checkStatsRequirement(const Item& item) const;
        bool isFit(const Item& item, const EquipTarget& target) const;
        auto needsToBeExchanged(const Item& item, const EquipTarget& target) const -> ExchangeResult;
        EquipTarget avoidLinks(const EquipTarget& target);
        Item takeOut(const EquipTarget& target);
        void layItem(const Item& item, int i, int j);
        bool autoPlaceItem(const Item& item, boost::optional<std::pair<xorder, yorder>> override_order = boost::none);
        bool exchangeWithCursor(EquipTarget takeoutTarget, boost::optional<EquipTarget> maybePlacementTarget);
        bool exchangeWithCursor(EquipTarget takeoutTarget);
        bool fitsAt(Item item, uint8_t x, uint8_t y);
        void removeItem(Item& item, Item::equipLoc from, uint8_t beltX = 0, uint8_t invX = 0, uint8_t invY = 0);
        static const uint8_t GOLD_PILE_MIN = 15;
        static const uint8_t GOLD_PILE_MID = 16;
        static const uint8_t GOLD_PILE_MAX = 17;
        friend class Player;
        friend class CharacterStatsBase;
    };
}
