#pragma once
#include "equiptarget.h"
#include <cstdint>
#include <faworld/item/item.h>
#include <functional>
#include <map>
#include <misc/array2d.h>
#include <misc/misc.h>
#include <optional>
#include <set>

namespace FAWorld
{
    class EquipTarget;
    class Actor;
    class ItemFactory;
    struct ItemStats;

    struct BasicInventoryBox
    {
        Vec2i topLeft = Vec2i::invalid();
        Vec2i position = Vec2i::invalid();
        Item* item = nullptr;
        bool isReal = false;
    };

    struct PlaceItemResult
    {
        enum Type
        {
            BlockedByItems,
            OutOfBounds,
            Success
        };

        Type type;
        std::set<BasicInventoryBox*> blockingItems;

        bool succeeded() const { return type == Type::Success; }
    };

    // A representation of the weapons / shield a player has equipped,
    // but normalised so we don't need to check left/right everywhere
    struct EquippedInHandsItems
    {
        struct TypeData
        {
            NonNullConstPtr<EquipmentItem> item;
            EquipTargetType location;
        };

        std::optional<TypeData> weapon;

        // will contain the same value as weapon above, if the type matches
        std::optional<TypeData> meleeWeapon;
        std::optional<TypeData> rangedWeapon;

        std::optional<TypeData> shield;
    };

    /// A Simple grid inventory, with no concept of "equipping" gear
    class BasicInventory
    {
    public:
        BasicInventory(int32_t width, int32_t height, bool treatAllItemsAs1By1 = false);

        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);

        bool canFitItem(const Item& item) const;
        bool autoPlaceItem(std::unique_ptr<Item>& item);
        PlaceItemResult placeItem(std::unique_ptr<Item>& item, int32_t x, int32_t y);

        /// attempts to place item at x,y, swapping the item pointer with whatever is in the way.
        /// Fails if there is more than one item in the way, or location would be out of bounds.
        /// returns boolean success or failure. Item can be empty.
        bool swapItem(std::unique_ptr<Item>& item, int32_t x, int32_t y);

        const Item* getItem(int32_t x, int32_t y) const { return mInventoryBox.get(x, y).item; }
        std::unique_ptr<Item> remove(int32_t x, int32_t y);

        int32_t width() const { return mInventoryBox.width(); }
        int32_t height() const { return mInventoryBox.height(); }

        class const_iterator
        {
        public:
            const_iterator() = default;
            const_iterator(const BasicInventory* sourceInventory, const BasicInventoryBox* it) : mSourceInventory(sourceInventory), mIt(it)
            {
                if (mIt != mSourceInventory->mInventoryBox.end() && !mIt->isReal)
                    this->operator++();
            }
            const_iterator(const const_iterator&) = default;
            const_iterator& operator=(const const_iterator&) = default;

            bool operator!=(const_iterator& other) const { return mSourceInventory != other.mSourceInventory || mIt != other.mIt; }
            const_iterator operator++()
            {
                const_iterator tmp = *this;

                if (mIt != mSourceInventory->mInventoryBox.end())
                {
                    do
                    {
                        mIt++;
                    } while (mIt != mSourceInventory->mInventoryBox.end() && !mIt->isReal);
                }

                return tmp;
            }

            const BasicInventoryBox& operator*() const { return *mIt; }

        public:
            const BasicInventory* mSourceInventory = nullptr;
            const BasicInventoryBox* mIt = nullptr;
        };

        const_iterator begin() const { return const_iterator(this, mInventoryBox.begin()); }
        const_iterator end() const { return const_iterator(this, mInventoryBox.end()); }

        std::function<void(const Item* removed, const Item* added)> mInventoryChanged;

    private:
        Misc::Array2D<BasicInventoryBox> mInventoryBox;
        bool mTreatAllItemsAs1by1 = false;
    };

    /// An ingame inventory, with slots for gear, and a main inventory, made up of BasicInventory instances
    class CharacterInventory
    {
    public:
        CharacterInventory();
        void save(FASaveGame::GameSaver& saver) const;
        void load(FASaveGame::GameLoader& loader);

        bool autoPlaceItem(std::unique_ptr<Item>&& item);
        bool autoPlaceItem(std::unique_ptr<Item>& item);
        bool forcePlaceItem(std::unique_ptr<Item>& item, const EquipTarget& target); /// no checks except bounds, just overwrites whatever is there
        const Item* getItemAt(const EquipTarget& target) const;
        std::unique_ptr<Item> remove(const EquipTarget& target);

        void setCursorHeld(std::unique_ptr<Item>&& item);

        void calculateItemBonuses(ItemStats& stats) const;
        bool isRangedWeaponEquipped() const;
        bool isShieldEquipped() const;
        EquippedInHandsItems getItemsInHands() const;

        const Item* getHead() const { return mHead.getItem(0, 0); }
        const Item* getBody() const { return mBody.getItem(0, 0); }
        const Item* getLeftRing() const { return mLeftRing.getItem(0, 0); }
        const Item* getRightRing() const { return mRightRing.getItem(0, 0); }
        const Item* getAmulet() const { return mAmulet.getItem(0, 0); }
        const Item* getLeftHand() const { return mLeftHand.getItem(0, 0); }
        const Item* getRightHand() const { return mRightHand.getItem(0, 0); }
        const Item* getCursorHeld() const { return mCursorHeld.getItem(0, 0); }

        const BasicInventory& getInv(EquipTargetType type) const;

        void slotClicked(const EquipTarget& slot);

        /// Places gold, combining piles up to max pile amount. If total quantity can't fit, returns the remainder
        int32_t placeGold(int32_t quantity, const ItemFactory& itemFactory);
        bool canFitGold(int32_t quantity) const;
        void takeOutGold(int32_t quantity);
        void splitGoldIntoCursor(int32_t x, int32_t y, int32_t amountToTransferToCursor, const ItemFactory& itemFactory);
        int32_t getTotalGold() const;

    private:
        BasicInventory& getInvMutable(EquipTargetType type);

    public:
        // This is not serialised - it should be reconnected by other means
        std::function<void(EquipTargetType inventoryType, const Item* removed, const Item* added)> mInventoryChanged;

    private:
        static constexpr int32_t inventoryWidth = 10;
        static constexpr int32_t inventoryHeight = 4;
        static constexpr int32_t beltWidth = 8;

        BasicInventory mMainInventory = BasicInventory(inventoryWidth, inventoryHeight);
        BasicInventory mBelt = BasicInventory(beltWidth, 1);
        BasicInventory mHead = BasicInventory(1, 1, true);
        BasicInventory mBody = BasicInventory(1, 1, true);
        BasicInventory mLeftRing = BasicInventory(1, 1, true);
        BasicInventory mRightRing = BasicInventory(1, 1, true);
        BasicInventory mAmulet = BasicInventory(1, 1, true);
        BasicInventory mLeftHand = BasicInventory(1, 1, true);
        BasicInventory mRightHand = BasicInventory(1, 1, true);
        BasicInventory mCursorHeld = BasicInventory(1, 1, true);

        std::map<EquipTargetType, BasicInventory&> mInventoryTypes = {{EquipTargetType::inventory, mMainInventory},
                                                                      {EquipTargetType::belt, mBelt},
                                                                      {EquipTargetType::head, mHead},
                                                                      {EquipTargetType::body, mBody},
                                                                      {EquipTargetType::leftRing, mLeftRing},
                                                                      {EquipTargetType::rightRing, mRightRing},
                                                                      {EquipTargetType::leftHand, mLeftHand},
                                                                      {EquipTargetType::rightHand, mRightHand},
                                                                      {EquipTargetType::amulet, mAmulet},
                                                                      {EquipTargetType::cursor, mCursorHeld}};
    };
}
