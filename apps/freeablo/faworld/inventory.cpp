#include "inventory.h"
#include "../fagui/guimanager.h"
#include "../fasavegame/gameloader.h"
#include "actorstats.h"
#include "equiptarget.h"
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

namespace FAWorld
{
    struct ExchangeResult
    {
        std::set<EquipTarget> NeedsToBeReplaced;
        std::set<EquipTarget> NeedsToBeReturned; // used only for equipping 2-handed weapon while wearing 1h weapon + shield
        boost::optional<EquipTarget> newTarget;  // sometimes target changes during exchange
        ExchangeResult(std::set<EquipTarget> NeedsToBeReplacedArg,
                       std::set<EquipTarget> NeedsToBeReturnedArg,
                       const boost::optional<EquipTarget>& newTargetArg);
    };

    ExchangeResult::ExchangeResult(std::set<EquipTarget> NeedsToBeReplacedArg = {},
                                   std::set<EquipTarget> NeedsToBeReturnedArg = {},
                                   const boost::optional<EquipTarget>& newTargetArg = {})
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

    bool Inventory::isFit(const Item& item, const EquipTarget& target) const
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
                    quantity -= toPlace;
                    if (quantity == 0)
                        return;
                }
            }
    }

    std::set<EquipTargetType> wearable = {
        EquipTargetType::leftHand,
        EquipTargetType::rightHand,
        EquipTargetType::leftRing,
        EquipTargetType::rightRing,
        EquipTargetType::amulet,
        EquipTargetType::body,
        EquipTargetType::head,
    };

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

    bool Inventory::autoPlaceItem(const Item& item, boost::optional<std::pair<Inventory::xorder, Inventory::yorder>> override_order)
    {
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
        if (item.getEquipLoc() == ItemEquipType::oneHanded && item.getClass() == ItemClass::weapon)
            for (auto hand_ptr : {&leftHand, &rightHand})
                if (hand_ptr->isEmpty())
                {
                    *hand_ptr = item;
                    equipChanged();
                    return true;
                }

        // different orders of placement for different item types as found in original game:
        auto requiredXOrder = xorder::fromLeft;
        auto requiredYOrder = yorder::fromTop;
        switch (item.getClass())
        {
            case ItemClass::armor:
                if (item.getEquipLoc() == ItemEquipType::oneHanded)
                    requiredXOrder = xorder::fromRight;
                break;
            case ItemClass::jewelryAndConsumable: // TODO: scrolls
                requiredYOrder = yorder::fromBottom;
                break;
            case ItemClass::gold:
                requiredYOrder = yorder::fromBottom;
                requiredXOrder = xorder::fromRight;
                break;
            default:
                break;
        }
        if (override_order)
        {
            requiredXOrder = override_order->first;
            requiredYOrder = override_order->second;
        }
        boost::any_range<int32_t, boost::single_pass_traversal_tag, int32_t, std::ptrdiff_t> xrange, yrange;
        // that's a bit slow technique in general but I think it's fine
        if (requiredXOrder == xorder::fromLeft)
            xrange = boost::irange(0, mInventoryBox.width(), 1);
        else
            xrange = boost::irange(mInventoryBox.width() - 1, -1, -1);
        if (requiredYOrder == yorder::fromTop)
            yrange = boost::irange(0, mInventoryBox.height(), 1);
        else
            yrange = boost::irange(mInventoryBox.height() - 1, -1, -1);
        for (int32_t x : xrange)
        {
            if (x + item.getInvSize()[0] > mInventoryBox.width())
                continue;
            for (int32_t y : yrange)
            {
                if (y + item.getInvSize()[1] > mInventoryBox.height())
                    continue;

                if (![&] {
                        for (int32_t xx = x; xx < x + item.getInvSize()[0]; ++xx)
                            for (int32_t yy = y; yy < y + item.getInvSize()[1]; ++yy)
                                if (!mInventoryBox.get(xx, yy).isEmpty())
                                    return false;
                        return true;
                    }())
                    continue;

                layItem(item, x, y);
                return true;
            }
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

        if (wearable.count(placementTarget.type) > 0 && !checkStatsRequirement(item))
            return false;

        if (!isFit(item, placementTarget))
            return false;

        auto requirements = needsToBeExchanged(item, placementTarget);
        if (requirements.NeedsToBeReplaced.size() > 1)
            return false;

        for (auto& location : requirements.NeedsToBeReturned)
        {
            // yes this is insanity but in this case order of auto placement differs from usual
            if (!autoPlaceItem(getItemAt(location), std::make_pair(xorder::fromLeft, yorder::fromTop)))
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

    void Inventory::inventoryMouseLeftButtonDown(double x, double y)
    {
        int32_t takeoutCellX = static_cast<int32_t>(x * mInventoryBox.width());
        int32_t takeoutCellY = static_cast<int32_t>(y * mInventoryBox.height());
        int32_t placementCellX = static_cast<int32_t>(x * mInventoryBox.width() - mCursorHeld.getInvSize()[0] * 0.5 + 0.5);
        int32_t placementCellY = static_cast<int32_t>(y * mInventoryBox.height() - mCursorHeld.getInvSize()[1] * 0.5 + 0.5);
        if (!isValidCell(takeoutCellX, takeoutCellY))
            return;
        exchangeWithCursor(MakeEquipTarget<EquipTargetType::inventory>(takeoutCellX, takeoutCellY),
                           MakeEquipTarget<EquipTargetType::inventory>(placementCellX, placementCellY));
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
