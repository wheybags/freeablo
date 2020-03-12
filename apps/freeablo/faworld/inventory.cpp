#include "inventory.h"
#include "../fagui/guimanager.h"
#include "../fasavegame/gameloader.h"
#include "../faworld/actorstats.h"
#include "actorstats.h"
#include "equiptarget.h"
#include "itemenums.h"
#include "itemfactory.h"
#include "player.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>

namespace FAWorld
{
    BasicInventory::BasicInventory(int32_t width, int32_t height, bool treatAllIItemsAs1by1)
        : mInventoryBox(width, height), mTreatAllItemsAs1by1(treatAllIItemsAs1by1)
    {
        for (int32_t y = 0; y < mInventoryBox.height(); y++)
        {
            for (int32_t x = 0; x < mInventoryBox.width(); x++)
            {
                mInventoryBox.get(x, y).mInvX = x;
                mInventoryBox.get(x, y).mInvY = y;
            }
        }
    }

    void BasicInventory::save(FASaveGame::GameSaver& saver) const
    {
        saver.save(mInventoryBox.width());
        saver.save(mInventoryBox.height());

        for (int32_t y = 0; y < mInventoryBox.height(); y++)
        {
            for (int32_t x = 0; x < mInventoryBox.width(); x++)
                mInventoryBox.get(x, y).save(saver);
        }

        saver.save(mTreatAllItemsAs1by1);
    }

    void BasicInventory::load(FASaveGame::GameLoader& loader)
    {
        int32_t width = loader.load<int32_t>();
        int32_t height = loader.load<int32_t>();

        mInventoryBox = Misc::Array2D<Item>(width, height);

        for (int32_t y = 0; y < mInventoryBox.height(); y++)
        {
            for (int32_t x = 0; x < mInventoryBox.width(); x++)
                mInventoryBox.get(x, y).load(loader);
        }

        mTreatAllItemsAs1by1 = loader.load<bool>();
    }

    bool BasicInventory::canFitItem(const Item& item) const
    {
        if (item.getType() == ItemType::gold)
        {
            const auto maxGoldPerSlot = item.getMaxCount();
            int32_t capacity = 0;

            for (const auto& slot : mInventoryBox.getFlatVector())
            {
                if (slot.isEmpty())
                    capacity += maxGoldPerSlot;
                else if (slot.getType() == ItemType::gold)
                    capacity += maxGoldPerSlot - slot.mCount;
            }

            return item.mCount <= capacity;
        }

        Misc::Point itemSize{item.getInvSize()[0], item.getInvSize()[1]};
        if (mTreatAllItemsAs1by1)
            itemSize = Misc::Point{1, 1};

        for (int32_t y = 0; y < mInventoryBox.height() - itemSize.y; y++)
        {
            for (int32_t x = 0; x < mInventoryBox.width() - itemSize.x; x++)
            {
                bool success = true;

                for (int32_t yy = y; yy < y + itemSize.y; yy++)
                {
                    for (int32_t xx = x; xx < x + itemSize.x; xx++)
                    {
                        auto& cell = mInventoryBox.get(xx, yy);

                        if (!cell.isEmpty())
                        {
                            success = false;
                            break;
                        }
                    }
                }

                if (success)
                    return true;
            }
        }

        return false;
    }

    bool BasicInventory::autoPlaceItem(Item& item, PlacementCheckOrder order)
    {
        // if (item.getType() == ItemType::gold)
        //    if (fillExistingGoldItems(item))
        //        return true;

        if (order == PlacementCheckOrder::Automatic)
        {
            order = PlacementCheckOrder::FromLeftTop;
            using sizeType = std::array<int32_t, 2>;
            if (item.getInvSize() == sizeType{1, 1})
                order = PlacementCheckOrder::FromLeftBottom;
            else if (item.getInvSize() == sizeType{2, 2})
                order = PlacementCheckOrder::SpecialFor2x2;
            else if (item.getInvSize() == sizeType{1, 2})
                order = PlacementCheckOrder::SpecialFor1x2;
        }

        switch (order)
        {
            case PlacementCheckOrder::FromLeftBottom:
                for (int32_t y = mInventoryBox.height() - 1; y != -1; y--)
                    for (int32_t x = 0; x != mInventoryBox.width(); x++)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                break;
            case PlacementCheckOrder::FromLeftTop:
                for (int32_t y = 0; y != mInventoryBox.height(); y++)
                    for (int32_t x = 0; x != mInventoryBox.width(); x++)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                break;
            case PlacementCheckOrder::FromRightBottom:
                for (int32_t y = mInventoryBox.height() - 1; y != -1; y--)
                    for (int32_t x = mInventoryBox.width() - 1; x != -1; x--)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                break;
            case PlacementCheckOrder::SpecialFor1x2:
                for (int32_t y = mInventoryBox.height() - 2; y != -1; y -= 2)
                    for (int32_t x = mInventoryBox.width() - 1; x != -1; x--)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                for (int32_t y = mInventoryBox.height() - 3; y != -1; y -= 2)
                    for (int32_t x = mInventoryBox.width() - 1; x != -1; x--)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                break;
            case PlacementCheckOrder::SpecialFor2x2:
                // this way lies madness
                for (int32_t x = mInventoryBox.width() - 2; x != -1; x -= 2)
                    for (int32_t y = 0; y != mInventoryBox.height(); x += 2)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                for (int32_t y = mInventoryBox.height() - 2; y != -1; y -= 2)
                    for (int32_t x = 1; x != mInventoryBox.width(); x += 2)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                for (int32_t y = 1; y != mInventoryBox.height(); y += 2)
                    for (int32_t x = 0; x != mInventoryBox.width(); x++)
                        if (placeItem(item, x, y).succeeded())
                            return true;
                break;
            default:
                invalid_enum(PlacementCheckOrder, order);
        }
        return false;
    }

    PlaceItemResult BasicInventory::placeItem(const Item& item, int32_t x, int32_t y)
    {
        Misc::Point itemSize{item.getInvSize()[0], item.getInvSize()[1]};
        if (mTreatAllItemsAs1by1)
            itemSize = Misc::Point{1, 1};

        // Shift item slot to the closest available that will fit the item.
        // This removes the need for "out of bounds" placement and gives a more intuitive feel
        // when placing an item near the edge of the inventory.
        x = std::max(x, 0);
        y = std::max(y, 0);
        x = std::min(x, mInventoryBox.width() - itemSize.x);
        y = std::min(y, mInventoryBox.height() - itemSize.y);

        std::set<Item*> blockingItems;

        for (int32_t yy = y; yy < y + itemSize.y; yy++)
        {
            for (int32_t xx = x; xx < x + itemSize.x; xx++)
            {
                auto& cell = mInventoryBox.get(xx, yy);

                if (!cell.isEmpty())
                {
                    auto& topLeftItemCell = mInventoryBox.get(cell.mCornerX, cell.mCornerY);
                    blockingItems.insert(&topLeftItemCell);
                }
            }
        }

        if (blockingItems.size() > 0)
            return PlaceItemResult{PlaceItemResult::Type::BlockedByItems, blockingItems};

        for (int32_t yy = y; yy < y + itemSize.y; yy++)
        {
            for (int32_t xx = x; xx < x + itemSize.x; xx++)
            {
                auto& cell = mInventoryBox.get(xx, yy);

                cell = item;
                cell.mIsReal = false;
                cell.mCornerX = x;
                cell.mCornerY = y;
                cell.mInvX = xx;
                cell.mInvY = yy;
            }
        }

        mInventoryBox.get(x, y).mIsReal = true;
        mInventoryChanged(Item(), item);

        return PlaceItemResult{PlaceItemResult::Type::Success, {}};
    }

    bool BasicInventory::swapItem(Item& item, int32_t x, int32_t y)
    {
        if (item.isEmpty())
        {
            item = remove(x, y);
            return item.isEmpty();
        }

        PlaceItemResult result = placeItem(item, x, y);

        switch (result.type)
        {
            case PlaceItemResult::Type::Success:
            {
                item = {};
                return true;
            }
            case PlaceItemResult::Type::BlockedByItems:
            {
                if (result.blockingItems.size() == 1)
                {
                    auto tmp = item;
                    auto removeFrom = (*result.blockingItems.begin())->getCornerCoords();
                    item = remove(removeFrom.first, removeFrom.second);
                    auto finalPlaceResult = placeItem(tmp, x, y);
                    release_assert(finalPlaceResult.succeeded());
                    return true;
                }

                return false;
            }
            case PlaceItemResult::Type::OutOfBounds:
            {
                return false;
            }
        }

        invalid_enum(PlaceItemResult, result.type);
    }

    Item BasicInventory::remove(int32_t x, int32_t y)
    {
        Item result = getItem(x, y);

        if (result.isEmpty())
            return {};

        Misc::Point itemSize{result.getInvSize()[0], result.getInvSize()[1]};
        if (mTreatAllItemsAs1by1)
            itemSize = Misc::Point{1, 1};

        for (int yLocal = result.getCornerCoords().second; yLocal < result.getCornerCoords().second + itemSize.y; ++yLocal)
            for (int xLocal = result.getCornerCoords().first; xLocal < result.getCornerCoords().first + itemSize.x; ++xLocal)
                mInventoryBox.get(xLocal, yLocal) = {};

        mInventoryChanged(result, Item());

        return result;
    }

    struct ExchangeResult
    {
        std::set<EquipTarget> NeedsToBeReplaced;
        std::set<EquipTarget> NeedsToBeReturned; // used only for equipping 2-handed weapon while wearing 1h weapon + shield
        std::optional<EquipTarget> newTarget;    // sometimes target changes during exchange
        ExchangeResult(std::set<EquipTarget> NeedsToBeReplacedArg = {},
                       std::set<EquipTarget> NeedsToBeReturnedArg = {},
                       const std::optional<EquipTarget>& newTargetArg = {});
    };

    ExchangeResult::ExchangeResult(std::set<EquipTarget> NeedsToBeReplacedArg,
                                   std::set<EquipTarget> NeedsToBeReturnedArg,
                                   const std::optional<EquipTarget>& newTargetArg)
        : NeedsToBeReplaced(std::move(NeedsToBeReplacedArg)), NeedsToBeReturned(std::move(NeedsToBeReturnedArg)), newTarget(newTargetArg)
    {
    }

    CharacterInventory::CharacterInventory()
    {
        for (const auto& pair : mInventoryTypes)
        {
            EquipTargetType type = pair.first;
            pair.second.mInventoryChanged = [this, type](Item const& removed, Item const& added) { mInventoryChanged(type, removed, added); };
        }
    }

    void CharacterInventory::save(FASaveGame::GameSaver& saver) const
    {
        mMainInventory.save(saver);
        mBelt.save(saver);
        mHead.save(saver);
        mBody.save(saver);
        mLeftRing.save(saver);
        mRightRing.save(saver);
        mAmulet.save(saver);
        mLeftHand.save(saver);
        mRightHand.save(saver);
        mCursorHeld.save(saver);
    }

    void CharacterInventory::load(FASaveGame::GameLoader& loader)
    {
        mMainInventory.load(loader);
        mBelt.load(loader);
        mHead.load(loader);
        mBody.load(loader);
        mLeftRing.load(loader);
        mRightRing.load(loader);
        mAmulet.load(loader);
        mLeftHand.load(loader);
        mRightHand.load(loader);
        mCursorHeld.load(loader);
    }

    bool CharacterInventory::autoPlaceItem(const Item& item)
    {
        Item tmp = item;
        return autoPlaceItem(tmp);
    }

    bool CharacterInventory::autoPlaceItem(Item& item, PlacementCheckOrder order)
    {
        // auto-placing in belt
        if (item.isBeltEquippable() && mBelt.autoPlaceItem(item, order))
            return true;

        // auto-equipping two handed weapons
        const Item& leftHand = mLeftHand.getItem(0, 0);
        const Item& rightHand = mRightHand.getItem(0, 0);
        if (item.getEquipLoc() == ItemEquipType::twoHanded && leftHand.isEmpty() && rightHand.isEmpty())
        {
            release_assert(mLeftHand.autoPlaceItem(item));
            release_assert(mRightHand.autoPlaceItem(item));
            return true;
        }

        // auto equip one handed weapons
        if (item.getEquipLoc() == ItemEquipType::oneHanded && item.getClass() == ItemClass::weapon && leftHand.isEmpty())
        {
            mLeftHand.autoPlaceItem(item);
            return true;
        }

        return mMainInventory.autoPlaceItem(item, order);
    }

    bool CharacterInventory::forcePlaceItem(const Item& item, const EquipTarget& target)
    {
        BasicInventory& inv = getInvMutable(target.type);
        PlaceItemResult result = inv.placeItem(item, target.posX, target.posY);

        switch (result.type)
        {
            case PlaceItemResult::Type::Success:
                return true;

            case PlaceItemResult::Type::OutOfBounds:
                return false;

            case PlaceItemResult::Type::BlockedByItems:
            {
                for (const Item* block : result.blockingItems)
                    inv.remove(block->getCornerCoords().first, block->getCornerCoords().second);

                result = inv.placeItem(item, target.posX, target.posY);
                release_assert(result.succeeded());
                return true;
            }
        }

        invalid_enum(PlaceItemResult, result.type);
    }

    const Item& CharacterInventory::getItemAt(const EquipTarget& target) const { return getInv(target.type).getItem(target.posX, target.posY); }

    Item CharacterInventory::remove(const EquipTarget& target) { return getInvMutable(target.type).remove(target.posX, target.posY); }

    void CharacterInventory::setCursorHeld(const Item& item)
    {
        mCursorHeld.remove(0, 0);
        mCursorHeld.placeItem(item, 0, 0).succeeded();
    }

    const BasicInventory& CharacterInventory::getInv(EquipTargetType type) const { return mInventoryTypes.at(type); }

    BasicInventory& CharacterInventory::getInvMutable(EquipTargetType type) { return mInventoryTypes.at(type); }

    void CharacterInventory::slotClicked(const EquipTarget& slot)
    {
        const Item& cursor = getCursorHeld();

        if (!cursor.isEmpty())
        {
            bool ok = true;

            switch (slot.type)
            {
                case EquipTargetType::inventory:
                    break;
                case EquipTargetType::belt:
                    ok = cursor.isBeltEquippable();
                    break;
                case EquipTargetType::head:
                    ok = cursor.getEquipLoc() == ItemEquipType::head;
                    break;
                case EquipTargetType::body:
                    ok = cursor.getEquipLoc() == ItemEquipType::chest;
                    break;
                case EquipTargetType::leftRing:
                case EquipTargetType::rightRing:
                    ok = cursor.getEquipLoc() == ItemEquipType::ring;
                    break;
                case EquipTargetType::leftHand:
                    ok = (cursor.getEquipLoc() == ItemEquipType::oneHanded && cursor.getClass() == ItemClass::weapon) ||
                         cursor.getEquipLoc() == ItemEquipType::twoHanded;
                    break;
                case EquipTargetType::rightHand:
                    ok = (cursor.getEquipLoc() == ItemEquipType::oneHanded && cursor.getClass() == ItemClass::armor) ||
                         cursor.getEquipLoc() == ItemEquipType::twoHanded;
                    break;
                case EquipTargetType::amulet:
                    ok = cursor.getEquipLoc() == ItemEquipType::amulet;
                    break;
                default:
                    invalid_enum(EquipTargetType, slot.type);
            }

            if (!ok)
                return;

            // Handle equipping two handed weapons.
            // We simply place the weapon in both hand slots.
            if (cursor.getEquipLoc() == ItemEquipType::twoHanded && (slot.type == EquipTargetType::leftHand || slot.type == EquipTargetType::rightHand))
            {
                if (getRightHand().isEmpty() || getLeftHand().isEmpty() || getLeftHand().getEquipLoc() == ItemEquipType::twoHanded)
                {
                    Item removed;
                    if (!getRightHand().isEmpty())
                        removed = mRightHand.remove(0, 0);
                    if (!getLeftHand().isEmpty())
                        removed = mLeftHand.remove(0, 0);

                    mLeftHand.placeItem(getCursorHeld(), 0, 0);
                    mRightHand.placeItem(getCursorHeld(), 0, 0);

                    setCursorHeld(removed);
                }

                return;
            }
        }

        BasicInventory& inv = getInvMutable(slot.type);

        Item tmp = mCursorHeld.getItem(0, 0);
        inv.swapItem(tmp, slot.posX, slot.posY);

        // when removing a two handed item, make sure we remove it from both hands
        if ((slot.type == EquipTargetType::leftHand || slot.type == EquipTargetType::rightHand) && tmp.getEquipLoc() == ItemEquipType::twoHanded)
            getInvMutable(slot.type == EquipTargetType::leftHand ? EquipTargetType::rightHand : EquipTargetType::leftHand).remove(0, 0);

        setCursorHeld(tmp);
    }

    void CharacterInventory::calculateItemBonuses(ItemStats& stats) const
    {
        EquipTarget hands[] = {MakeEquipTarget<EquipTargetType::leftHand>(), MakeEquipTarget<EquipTargetType::rightHand>()};
        for (auto& slot : hands)
        {
            const Item& item = getItemAt(slot);
            if (Item::isItemAMeleeWeapon(item.getType()))
                stats.meleeDamageBonusRange += {item.getMinAttackDamage(), item.getMaxAttackDamage()};
            else if (Item::isItemARangedWeapon(item.getType()))
                stats.rangedDamageBonusRange += {item.getMinAttackDamage(), item.getMaxAttackDamage()};

            // TODO: other stats
        }
    }

    bool CharacterInventory::isRangedWeaponEquipped() const
    {
        EquipTarget hands[] = {MakeEquipTarget<EquipTargetType::leftHand>(), MakeEquipTarget<EquipTargetType::rightHand>()};
        for (auto& slot : hands)
        {
            const Item& item = getItemAt(slot);
            if (Item::isItemARangedWeapon(item.getType()))
                return true;
        }

        return false;
    }

    bool CharacterInventory::isShieldEquipped() const { return getLeftHand().getType() == ItemType::shield || getRightHand().getType() == ItemType::shield; }

    EquippedInHandsItems CharacterInventory::getItemsInHands() const
    {
        EquippedInHandsItems retval = {};

        EquipTarget hands[] = {MakeEquipTarget<EquipTargetType::leftHand>(), MakeEquipTarget<EquipTargetType::rightHand>()};
        for (auto& slot : hands)
        {
            const Item& item = getItemAt(slot);

            if (Item::isItemARangedWeapon(item.getType()))
                retval.rangedWeapon = EquippedInHandsItems::TypeData{item, slot.type};
            else if (Item::isItemAMeleeWeapon(item.getType()))
                retval.meleeWeapon = EquippedInHandsItems::TypeData{item, slot.type};
            else if (item.getType() == ItemType::shield)
                retval.shield = EquippedInHandsItems::TypeData{item, slot.type};
        }

        return retval;
    }

    int32_t CharacterInventory::placeGold(int32_t quantity, const ItemFactory& itemFactory)
    {
        if (quantity == 0)
            return 0;

        // first part - filling existing gold piles
        for (auto& item : mMainInventory)
        {
            if (item.getType() != ItemType::gold)
                continue;
            int room = item.getMaxCount() - item.mCount;
            if (room > 0)
            {
                auto toPlace = std::min(quantity, room);

                Item copy(item);
                copy.mCount += toPlace;
                int32_t x = item.mInvX;
                int32_t y = item.mInvY;

                release_assert(!mMainInventory.remove(x, y).isEmpty());
                release_assert(mMainInventory.placeItem(copy, x, y).succeeded());

                quantity -= toPlace;
                if (quantity == 0)
                    return 0;
            }
        }
        // second part - filling the empty slots with gold
        for (int32_t x = 0; x != mMainInventory.width(); x++)
        {
            for (int32_t y = 0; y != mMainInventory.height(); x++)
            {
                if (mMainInventory.getItem(x, y).isEmpty())
                {
                    auto item = itemFactory.generateBaseItem(ItemId::gold);
                    auto toPlace = std::min(quantity, item.getMaxCount());
                    item.mCount = toPlace;
                    mMainInventory.placeItem(item, x, y);
                    quantity -= toPlace;
                    if (quantity == 0)
                        return 0;
                }
            }
        }

        return quantity;
    }

    void CharacterInventory::takeOutGold(int32_t quantity)
    {
        for (const Item& item : mMainInventory)
        {
            if (item.getType() != ItemType::gold)
                continue;

            Item copy = item;
            auto toTake = std::min(quantity, copy.mCount);
            copy.mCount -= toTake;
            quantity -= toTake;

            int32_t x = item.mInvX;
            int32_t y = item.mInvY;

            mMainInventory.remove(x, y);
            if (copy.mCount > 0)
            {
                PlaceItemResult result = mMainInventory.placeItem(copy, x, y);
                release_assert(result.succeeded());
            }

            if (quantity == 0)
                return;
        }

        message_and_abort("Not enough gold");
    }

    void CharacterInventory::splitGoldIntoCursor(int32_t x, int32_t y, int32_t amountToTransferToCursor, const ItemFactory& itemFactory)
    {
        Item goldFromInventoryItem = mMainInventory.remove(x, y);
        release_assert(goldFromInventoryItem.mBaseId == ItemId::gold);

        amountToTransferToCursor = std::min(goldFromInventoryItem.mCount, amountToTransferToCursor);
        goldFromInventoryItem.mCount -= amountToTransferToCursor;

        Item cursorGold = itemFactory.generateBaseItem(ItemId::gold);
        cursorGold.mCount = amountToTransferToCursor;

        setCursorHeld(cursorGold);
        if (goldFromInventoryItem.mCount > 0)
            mMainInventory.placeItem(goldFromInventoryItem, x, y);
    }

    int32_t CharacterInventory::getTotalGold() const
    {
        int32_t total = 0;
        for (const Item& item : mMainInventory)
        {
            if (item.getType() == ItemType::gold)
                total += item.mCount;
        }
        return total;
    }
}
