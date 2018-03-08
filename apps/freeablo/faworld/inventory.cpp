#include "inventory.h"
#include "../fagui/guimanager.h"
#include "../fasavegame/gameloader.h"
#include "actorstats.h"
#include "boost/container/flat_set.hpp"
#include "equiptarget.h"
#include "itembonus.h"
#include "itemenums.h"
#include "itemfactory.h"
#include "player.h"
#include <algorithm>
#include <boost/range/any_range.hpp>
#include <boost/range/irange.hpp>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>
using namespace boost::container;

namespace FAWorld
{
    struct ExchangeResult
    {
        std::set<EquipTarget> NeedsToBeReplaced;
        std::set<EquipTarget> NeedsToBeReturned; // used only for equipping 2-handed weapon while wearing 1h weapon + shield
        boost::optional<EquipTarget> newTarget;  // sometimes target changes during exchange
        ExchangeResult(std::set<EquipTarget> NeedsToBeReplacedArg = {},
                       std::set<EquipTarget> NeedsToBeReturnedArg = {},
                       const boost::optional<EquipTarget>& newTargetArg = {});
    };

    ExchangeResult::ExchangeResult(std::set<EquipTarget> NeedsToBeReplacedArg,
                                   std::set<EquipTarget> NeedsToBeReturnedArg,
                                   const boost::optional<EquipTarget>& newTargetArg)
        : NeedsToBeReplaced(std::move(NeedsToBeReplacedArg)), NeedsToBeReturned(std::move(NeedsToBeReturnedArg)), newTarget(newTargetArg)
    {
    }

    Inventory::Inventory()
    {
        for (uint8_t y = 0; y < mInventoryBox.height(); y++)
        {
            for (uint8_t x = 0; x < mInventoryBox.width(); x++)
            {
                mInventoryBox.get(x, y).mInvX = x;
                mInventoryBox.get(x, y).mInvY = y;
            }
        }
    }

    void Inventory::save(FASaveGame::GameSaver& saver)
    {
        saver.save(mInventoryBox.width());
        saver.save(mInventoryBox.height());

        for (int32_t y = 0; y < mInventoryBox.height(); y++)
            for (int32_t x = 0; x < mInventoryBox.width(); x++)
                mInventoryBox.get(x, y).save(saver);

        saver.save(uint32_t(mBelt.size()));

        for (Item& item : mBelt)
            item.save(saver);

        mHead.save(saver);
        mBody.save(saver);
        mLeftRing.save(saver);
        mRightRing.save(saver);
        mAmulet.save(saver);
        mLeftHand.save(saver);
        mRightHand.save(saver);
        mCursorHeld.save(saver);
    }

    void Inventory::load(FASaveGame::GameLoader& loader, const DiabloExe::DiabloExe& exe)
    {
        int32_t boxW = loader.load<int32_t>();
        int32_t boxH = loader.load<int32_t>();
        release_assert(boxW >= mInventoryBox.width() && boxH >= mInventoryBox.height());

        for (int32_t y = 0; y < boxH; y++)
            for (int32_t x = 0; x < boxW; x++)
                mInventoryBox.get(x, y).load(loader, exe);

        uint32_t beltSize = loader.load<uint32_t>();
        release_assert(beltSize >= mBelt.size());

        for (uint32_t i = 0; i < beltSize; i++)
            mBelt[i].load(loader, exe);

        for (auto ptr : {&mHead, &mBody, &mLeftRing, &mRightRing, &mAmulet, &mLeftHand, &mRightHand, &mCursorHeld})
            ptr->load(loader, exe);
    }

    bool Inventory::checkStatsRequirement(const Item& item) const
    {
        UNUSED_PARAM(item);
        /*if(!(mActor.mStats->getStrength() >= item.getReqStr() &&
             mActor.mStats->getDexterity() >= item.getReqDex() &&
             mActor.mStats->getMagic() >= item.getReqMagic() &&
             mActor.mStats->getVitality() >= item.getReqVit()))
            return false;
        else*/
        return true;
    }

    static const std::map<ItemEquipType, std::set<EquipTargetType>> appropriateLocations = {
        {ItemEquipType::oneHanded, {EquipTargetType::leftHand, EquipTargetType::rightHand}},
        {ItemEquipType::twoHanded, {EquipTargetType::leftHand, EquipTargetType::rightHand}},
        {ItemEquipType::ring, {EquipTargetType::leftRing, EquipTargetType::rightRing}},
        {ItemEquipType::amulet, {EquipTargetType::amulet}},
        {ItemEquipType::chest, {EquipTargetType::body}},
        {ItemEquipType::head, {EquipTargetType::head}},
    };

    bool Inventory::couldBePlacedToInventory(const Item& item, const EquipTarget& target) const
    {
        switch (target.type)
        {
            case EquipTargetType::inventory:
                return target.posX + item.getInvSize()[0] <= mInventoryBox.width() && target.posY + item.getInvSize()[1] <= mInventoryBox.height();
            case EquipTargetType::belt:
                return item.isBeltEquippable();
            default:
            {
                auto it = appropriateLocations.find(item.getEquipLoc());
                if (it != appropriateLocations.end())
                    return it->second.count(target.type) > 0;
                break;
            }
        }
        return false;
    }

    auto Inventory::needsToBeExchanged(const Item& item, const EquipTarget& target) const -> ExchangeResult
    {
        switch (target.type)
        {
            case EquipTargetType::leftHand:
            case EquipTargetType::rightHand:
            {
                auto& thisHand = getItemAt(target);
                auto getOtherHand = [](const EquipTarget& target) -> EquipTarget {
                    if (target.type == EquipTargetType::leftHand)
                        return MakeEquipTarget<EquipTargetType::rightHand>();
                    else
                        return MakeEquipTarget<EquipTargetType::leftHand>();
                };
                auto& otherHand = getItemAt(getOtherHand(target));
                if (thisHand.getEquipLoc() == ItemEquipType::twoHanded)
                    return {{MakeEquipTarget<EquipTargetType::leftHand>()}, {}};
                if (otherHand.isEmpty())
                {
                    if (thisHand.isEmpty())
                        return {{}, {}};
                    else
                        return {{target}, {}};
                }
                // case where weapon and shield are equipped
                auto checkHand = [&](const EquipTarget& hand) -> boost::optional<ExchangeResult> {
                    auto& handItem = getItemAt(hand);
                    auto& otherHandItem = getItemAt(getOtherHand(hand));
                    if (item.getEquipLoc() == ItemEquipType::twoHanded)
                    {
                        // in this case we need to exchange with weapon and place shield back to inventory if possible
                        // if it's not possible then this item equipping should also be deemed impossible.
                        if (otherHandItem.isEmpty())
                            return ExchangeResult{{hand}, {}};
                        else if (handItem.getClass() == ItemClass::weapon)
                            return ExchangeResult{{hand}, {getOtherHand(hand)}};
                    }
                    if (handItem.getClass() == item.getClass())
                    {
                        // if it's shield, it is replaced with shield, if it's 1h weapon, it's replaced with it
                        // no matter which slot we clicked
                        return ExchangeResult{{hand}, {}, hand};
                    }
                    return {};
                };
                if (auto res = checkHand(MakeEquipTarget<EquipTargetType::leftHand>()))
                    return *res;
                if (auto res = checkHand(MakeEquipTarget<EquipTargetType::rightHand>()))
                    return *res;
                break;
            }
            case EquipTargetType::inventory:
            {
                ExchangeResult result;
                for (int32_t y = target.posY; y < target.posY + item.getInvSize()[1]; y++)
                {
                    for (int32_t x = target.posX; x < target.posX + item.getInvSize()[0]; x++)
                    {
                        if (!mInventoryBox.get(x, y).isEmpty())
                        {
                            auto cornerCoords = mInventoryBox.get(x, y).getCornerCoords();
                            result.NeedsToBeReplaced.insert(MakeEquipTarget<EquipTargetType::inventory>(cornerCoords.first, cornerCoords.second));
                        }
                    }
                }

                return result;
            }
            default:
                if (!getItemAt(target).isEmpty())
                    return {{target}, {}};
        }
        return {{}, {}};
    }

    EquipTarget Inventory::avoidLinks(const EquipTarget& target)
    {
        switch (target.type)
        {
            case EquipTargetType::inventory:
            {
                auto& item = getItemAt(target);
                return MakeEquipTarget<EquipTargetType::inventory>(item.getCornerCoords().first, item.getCornerCoords().second);
            }
            default:
                return target;
        }
    }

    Item Inventory::takeOut(const EquipTarget& target)
    {
        auto realTarget = avoidLinks(target);
        auto copy = getItemAt(realTarget);
        if (copy.getEquipLoc() == ItemEquipType::twoHanded)
        {
            if (target.type == EquipTargetType::leftHand)
                getItemAt(MakeEquipTarget<EquipTargetType::rightHand>()) = {};
            else if (target.type == EquipTargetType::rightHand)
                return {}; // Cancel the operation, you can't click on illusionary two-hand item on the right in diablo
        }
        switch (target.type)
        {
            case EquipTargetType::inventory:
                for (int y = realTarget.posY; y < realTarget.posY + copy.getInvSize()[1]; ++y)
                    for (int x = realTarget.posX; x < realTarget.posX + copy.getInvSize()[0]; ++x)
                        mInventoryBox.get(x, y) = {};
                break;
            default:
                getItemAt(realTarget) = {};
        }
        return copy;
    }

    void Inventory::placeGold(int quantity, const ItemFactory& itemFactory)
    {
        if (quantity == 0)
            return;

        // first part - filling existing gold piles
        for (auto& item : mInventoryBox)
        {
            if (item.getType() != ItemType::gold)
                continue;
            int room = item.getMaxCount() - item.mCount;
            if (room > 0)
            {
                auto toPlace = std::min(quantity, room);
                item.mCount += toPlace;
                quantity -= toPlace;
                if (quantity == 0)
                    return;
            }
        }
        // second part - filling the empty slots with gold
        for (auto x : boost::irange(0, inventoryWidth, 1))
            for (auto y : boost::irange(0, inventoryHeight, 1))
            {
                auto target = MakeEquipTarget<EquipTargetType::inventory>(x, y);
                if (getItemAt(target).isEmpty())
                {
                    auto item = itemFactory.generateBaseItem(ItemId::gold);
                    auto toPlace = std::min(quantity, item.getMaxCount());
                    item.mCount = toPlace;
                    putItemUnsafe(item, target);
                    quantity -= toPlace;
                    if (quantity == 0)
                        return;
                }
            }
    }

    static const EquipTarget slotEquipTargets[] = {MakeEquipTarget<EquipTargetType::leftHand>(),
                                                   MakeEquipTarget<EquipTargetType::rightHand>(),
                                                   MakeEquipTarget<EquipTargetType::leftRing>(),
                                                   MakeEquipTarget<EquipTargetType::rightRing>(),
                                                   MakeEquipTarget<EquipTargetType::amulet>(),
                                                   MakeEquipTarget<EquipTargetType::body>(),
                                                   MakeEquipTarget<EquipTargetType::head>()};

    static const flat_set<EquipTarget> equipSlotsSet(std::begin(slotEquipTargets), std::end(slotEquipTargets));

    ItemBonus Inventory::getTotalItemBonus() const
    {
        ItemBonus total;
        for (auto slot : slotEquipTargets)
        {
            auto& item = getItemAt(slot);
            if (!item.isEmpty())
            {
                // TODO: add stat recheck, because item may become invalid while equipped in Diablo thus becoming useless
                total += item.getBonus();
            }
        }
        if (total.minAttackDamage == 0 && total.maxAttackDamage == 0)
        {
            total.minAttackDamage = total.maxAttackDamage = 1;
            // TODO: stat recheck for shield
            if (getItemAt(MakeEquipTarget<EquipTargetType::leftHand>()).getType() == ItemType::shield ||
                getItemAt(MakeEquipTarget<EquipTargetType::rightHand>()).getType() == ItemType::shield)
                total.maxAttackDamage += 2;
        }
        return total;
    }

    void Inventory::takeOutGold(int32_t quantity)
    {
        for (auto row : boost::irange(mInventoryBox.height() - 1, -1, -1))
            for (auto column : boost::irange(mInventoryBox.width() - 1, -1, -1))
            {
                auto& item = mInventoryBox.get(column, row);
                if (item.getType() != ItemType::gold)
                    continue;
                auto toTake = std::min(quantity, item.mCount);
                item.mCount -= toTake;
                quantity -= toTake;
                if (item.mCount == 0)
                    item = {};
                if (quantity == 0)
                    return;
            }
        release_assert("Not enough gold");
    }

    bool Inventory::couldBePlacedToInventory(const Item& item) const
    {
        for (auto x : boost::irange(0, inventoryWidth, 1))
            for (auto y : boost::irange(0, inventoryHeight, 1))
                if (couldBePlacedToInventory(item, x, y))
                    return true;
        return false;
    }

    void Inventory::putItemUnsafe(const Item& item, const EquipTarget& target)
    {
        if (item.getEquipLoc() == ItemEquipType::twoHanded && target.type == EquipTargetType::leftHand)
        {
            auto& rightHand = getItemAt(MakeEquipTarget<EquipTargetType::rightHand>());
            rightHand = item;
            rightHand.mIsReal = false;
        }
        switch (target.type)
        {
            default:
                getItemAt(target) = item;
                break;
            case EquipTargetType::inventory:
                layItem(item, target.posX, target.posY);
                break;
        }
    }

    bool Inventory::fillExistingGoldItems(Item& goldItem)
    {
        // TODO: write unit tests for inventory handling
        release_assert(goldItem.getType() == ItemType::gold);
        for (int32_t y = 0; y < inventoryHeight; ++y)
            for (int32_t x = 0; x < inventoryWidth; ++x)
            {
                auto& item = mInventoryBox.get(x, y);
                if (item.getType() == ItemType::gold)
                {
                    auto amount = std::min(goldItem.mCount, item.getMaxCount() - item.mCount);
                    goldItem.mCount -= amount;
                    item.mCount += amount;
                    if (goldItem.mCount == 0)
                        return true;
                }
            }
        return goldItem.mCount == 0;
    }

    void Inventory::layItem(const Item& item, int32_t x, int32_t y)
    {
        for (int32_t yy = y; yy < y + item.getInvSize()[1]; yy++)
        {
            for (int32_t xx = x; xx < x + item.getInvSize()[0]; xx++)
            {
                auto& cell = mInventoryBox.get(xx, yy);
                cell = item;
                cell.mIsReal = false;
                cell.mCornerX = x;
                cell.mCornerY = y;
            }
        }

        mInventoryBox.get(x, y).mIsReal = true;
    }

    bool Inventory::couldBePlacedToInventory(const Item& item, int32_t x, int32_t y) const
    {
        if (x + item.getInvSize()[0] > mInventoryBox.width())
            return false;
        if (y + item.getInvSize()[1] > mInventoryBox.height())
            return false;

        if (![&] {
                for (int32_t xx = x; xx < x + item.getInvSize()[0]; ++xx)
                    for (int32_t yy = y; yy < y + item.getInvSize()[1]; ++yy)
                        if (!mInventoryBox.get(xx, yy).isEmpty())
                            return false;
                return true;
            }())
            return false;

        return true;
    }

    bool Inventory::tryPlace(const Item& item, int32_t x, int32_t y)
    {
        if (!couldBePlacedToInventory(item, x, y))
            return false;
        layItem(item, x, y);
        return true;
    }

    bool Inventory::autoPlaceItem(Item& item, boost::optional<PlacementCheckOrder> overrideOrder)
    {
        if (item.getType() == ItemType::gold)
            if (fillExistingGoldItems(item))
                return true;
        // auto-placing in belt
        if (item.isBeltEquippable())
            for (int32_t i = 0; i < int32_t(mBelt.size()); ++i)
            {
                auto& place = getItemAt(MakeEquipTarget<EquipTargetType::belt>(i));
                if (place.isEmpty())
                {
                    place = item;
                    return true;
                }
            }
        // auto-equipping weapons
        auto& leftHand = getItemAt(MakeEquipTarget<EquipTargetType::leftHand>());
        auto& rightHand = getItemAt(MakeEquipTarget<EquipTargetType::rightHand>());
        if (item.getEquipLoc() == ItemEquipType::twoHanded && leftHand.isEmpty() && rightHand.isEmpty())
        {
            putItemUnsafe(item, MakeEquipTarget<EquipTargetType::leftHand>());
            equipChanged();
            return true;
        }
        // only for weapons, not shields
        if (item.getEquipLoc() == ItemEquipType::oneHanded && item.getClass() == ItemClass::weapon && leftHand.getClass() != ItemClass::weapon &&
            rightHand.getClass() != ItemClass::weapon)
            for (auto hand_ptr : {&leftHand, &rightHand})
                if (hand_ptr->isEmpty())
                {
                    *hand_ptr = item;
                    equipChanged();
                    return true;
                }

        auto order = PlacementCheckOrder::fromLeftTop;
        using sizeType = std::array<int32_t, 2>;
        if (item.getInvSize() == sizeType{1, 1})
            order = PlacementCheckOrder::fromLeftBottom;
        else if (item.getInvSize() == sizeType{2, 2})
            order = PlacementCheckOrder::specialFor2x2;
        else if (item.getInvSize() == sizeType{1, 2})
            order = PlacementCheckOrder::specialFor1x2;
        if (overrideOrder)
        {
            order = *overrideOrder;
        }
        switch (order)
        {
            case PlacementCheckOrder::fromLeftBottom:
                for (auto y : boost::irange(inventoryHeight - 1, -1, -1))
                    for (auto x : boost::irange(0, inventoryWidth, 1))
                        if (tryPlace(item, x, y))
                            return true;
                break;
            case PlacementCheckOrder::fromLeftTop:
                for (auto y : boost::irange(0, inventoryHeight, 1))
                    for (auto x : boost::irange(0, inventoryWidth, 1))
                        if (tryPlace(item, x, y))
                            return true;
                break;
            case PlacementCheckOrder::fromRightBottom:
                for (auto y : boost::irange(inventoryHeight - 1, -1, -1))
                    for (auto x : boost::irange(inventoryWidth - 1, -1, -1))
                        if (tryPlace(item, x, y))
                            return true;
                break;
            case PlacementCheckOrder::specialFor1x2:
                for (auto y : boost::irange(inventoryHeight - 2, -1, -2))
                    for (auto x : boost::irange(inventoryWidth - 1, -1, -1))
                        if (tryPlace(item, x, y))
                            return true;
                for (auto y : boost::irange(inventoryHeight - 3, -1, -2))
                    for (auto x : boost::irange(inventoryWidth - 1, -1, -1))
                        if (tryPlace(item, x, y))
                            return true;
                break;
            case PlacementCheckOrder::specialFor2x2:
                // this way lies madness
                for (auto x : boost::irange(inventoryWidth - 2, -1, -2))
                    for (auto y : boost::irange(0, inventoryHeight, 2))
                        if (tryPlace(item, x, y))
                            return true;
                for (auto y : boost::irange(inventoryHeight - 2, -1, -2))
                    for (auto x : boost::irange(1, inventoryWidth, 2))
                        if (tryPlace(item, x, y))
                            return true;
                for (auto y : boost::irange(1, inventoryHeight, 2))
                    for (auto x : boost::irange(0, inventoryWidth, 1))
                        if (tryPlace(item, x, y))
                            return true;
                break;
        }
        return false;
    }

    bool Inventory::exchangeWithCursor(EquipTarget takeoutTarget, boost::optional<EquipTarget> maybePlacementTarget)
    {
        auto placementTarget = maybePlacementTarget ? *maybePlacementTarget : takeoutTarget;
        if (mCursorHeld.isEmpty())
        {
            if (getItemAt(takeoutTarget).isEmpty())
                return false;

            setCursorHeld(takeOut(takeoutTarget));
            return true;
        }

        auto& item = mCursorHeld;
        if (item.getEquipLoc() == ItemEquipType::twoHanded && placementTarget.type == EquipTargetType::rightHand)
            placementTarget = MakeEquipTarget<EquipTargetType::leftHand>();

        if (equipSlotsSet.count(placementTarget) > 0 && !checkStatsRequirement(item))
            return false;

        if (!couldBePlacedToInventory(item, placementTarget))
            return false;

        auto requirements = needsToBeExchanged(item, placementTarget);
        if (requirements.NeedsToBeReplaced.size() > 1)
            return false;

        for (auto& location : requirements.NeedsToBeReturned)
        {
            // yes this is insanity but in this case order of auto placement differs from usual
            if (!autoPlaceItem(getItemAt(location), PlacementCheckOrder::fromLeftTop))
                return false;

            takeOut(location); // take out and discard
        }

        placementTarget = requirements.newTarget ? *requirements.newTarget : placementTarget;

        if (requirements.NeedsToBeReplaced.empty())
        {
            putItemUnsafe(item, placementTarget);
            setCursorHeld({});
            return true;
        }

        auto& exchangeeLocation = *requirements.NeedsToBeReplaced.begin();
        {
            auto& exchangeeRef = getItemAt(exchangeeLocation);
            if (exchangeeRef.getType() == ItemType::gold)
            {
                if (exchangeeRef.mCount < exchangeeRef.getMaxCount())
                {
                    auto amount = std::min(item.mCount, exchangeeRef.getMaxCount() - exchangeeRef.mCount);
                    exchangeeRef.mCount += amount;
                    item.mCount -= amount;
                    if (item.mCount == 0)
                        setCursorHeld({});
                    else
                        setCursorHeld(item);
                    return true;
                }
            }
        }
        auto exchangee = takeOut(exchangeeLocation);
        putItemUnsafe(item, placementTarget);
        setCursorHeld(exchangee);
        return true;
    }

    bool Inventory::exchangeWithCursor(EquipTarget takeoutTarget) { return exchangeWithCursor(takeoutTarget, boost::none); }

    void Inventory::itemSlotLeftMouseButtonDown(EquipTarget target)
    {
        if (exchangeWithCursor(target))
            equipChanged();
    }

    void Inventory::beltMouseLeftButtonDown(double x)
    {
        int beltX = static_cast<int32_t>(x * mBelt.size());
        exchangeWithCursor(MakeEquipTarget<EquipTargetType::belt>(beltX));
    }

    void Inventory::inventoryMouseLeftButtonDown(Misc::Point cell)
    {
        /*int32_t takeoutCellX = static_cast<int32_t>(x * mInventoryBox.width());
        int32_t takeoutCellY = static_cast<int32_t>(y * mInventoryBox.height());
        int32_t placementCellX = static_cast<int32_t>(x * mInventoryBox.width() - mCursorHeld.getInvSize()[0] * 0.5 + 0.5);
        int32_t placementCellY = static_cast<int32_t>(y * mInventoryBox.height() - mCursorHeld.getInvSize()[1] * 0.5 + 0.5);*/

        Misc::Point placementCell{static_cast<int32_t>(cell.x - mCursorHeld.getInvSize()[0] * 0.5 + 0.5),
                                  static_cast<int32_t>(cell.y - mCursorHeld.getInvSize()[1] * 0.5 + 0.5)};

        if (!isValidCell(cell.x, cell.y))
            return;

        exchangeWithCursor(MakeEquipTarget<EquipTargetType::inventory>(cell.x, cell.y),
                           MakeEquipTarget<EquipTargetType::inventory>(placementCell.x, placementCell.y));
    }

    void Inventory::setCursorHeld(const Item& item)
    {
        mCursorHeld = item;
        updateCursor();
    }

    std::vector<EquipTarget> Inventory::getBeltAndInventoryItemPositions() const
    {
        std::vector<EquipTarget> ret;
        auto check_target = [&](const EquipTarget& target) {
            auto item = getItemAt(target);
            if (item.mIsReal && !item.isEmpty())
                ret.push_back(target);
        };

        for (auto x : boost::irange(0, inventoryWidth, 1))
            for (auto y : boost::irange(0, inventoryHeight, 1))
                check_target(MakeEquipTarget<EquipTargetType::inventory>(x, y));
        for (auto x : boost::irange(0, beltWidth, 1))
            check_target(MakeEquipTarget<EquipTargetType::belt>(x));
        return ret;
    }

    void Inventory::updateCursor()
    {
        if (mCursorHeld.isEmpty())
        {
            FAGui::cursorFrame = 0;
            FAGui::cursorHotspot = Render::CursorHotspotLocation::topLeft;
        }
        else
        {
            FAGui::cursorFrame = mCursorHeld.getGraphicValue();
            FAGui::cursorHotspot = Render::CursorHotspotLocation::center;
        }
    }

    const Item& Inventory::getItemAt(const EquipTarget& target) const { return const_cast<Inventory*>(this)->getItemAt(target); }

    Item& Inventory::getItemAt(const EquipTarget& target)
    {
        switch (target.type)
        {
            case EquipTargetType::leftHand:
                return mLeftHand;
            case EquipTargetType::leftRing:
                return mLeftRing;
            case EquipTargetType::rightHand:
                return mRightHand;
            case EquipTargetType::rightRing:
                return mRightRing;
            case EquipTargetType::body:
                return mBody;
            case EquipTargetType::head:
                return mHead;
            case EquipTargetType::amulet:
                return mAmulet;
            case EquipTargetType::inventory:
                return mInventoryBox.get(target.posX, target.posY);
            case EquipTargetType::belt:
                return mBelt[target.posX];
            case EquipTargetType::cursor:
                return mCursorHeld;
            default:
                break;
        }
        release_assert(false);
    }
}
