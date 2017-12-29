#include "inventory.h"
#include "../fagui/guimanager.h"
#include "actorstats.h"
#include "itemmanager.h"
#include "player.h"
#include <algorithm>
#include <boost/range/irange.hpp>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>

#include "equiptarget.h"
#include "itemenums.h"
#include <boost/range/any_range.hpp>

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

    Inventory::Inventory(Player* actor)
    {
        mAttackDamageTotal = 0;
        mArmourClassTotal = 0;
        mActor = actor;
        for (uint8_t i = 0; i < 4; i++)
        {
            for (uint8_t j = 0; j < 10; j++)
            {
                mInventoryBox[i][j].mInvX = j;
                mInventoryBox[i][j].mInvY = i;
            }
        }
    }

    uint32_t Inventory::getTotalArmourClass()
    {
        if (mActor == NULL)
            return 0;
        return mArmourClassTotal;
    }

    uint32_t Inventory::getTotalAttackDamage() { return mAttackDamageTotal; }

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
                return target.posX + item.getInvSize().first <= inventoryWidth && target.posY + item.getInvSize().second <= inventoryHeight;
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
                for (auto i = target.posX; i < target.posX + item.getInvSize().first; ++i)
                    for (auto j = target.posY; j < target.posY + item.getInvSize().second; ++j)
                        if (!mInventoryBox[j][i].isEmpty())
                        {
                            auto cornerCoords = mInventoryBox[j][i].getCornerCoords();
                            result.NeedsToBeReplaced.insert(MakeEquipTarget<EquipTargetType::inventory>(cornerCoords.first, cornerCoords.second));
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
                for (int j = realTarget.posY; j < realTarget.posY + copy.getInvSize().second; ++j)
                    for (int i = realTarget.posX; i < realTarget.posX + copy.getInvSize().first; ++i)
                        mInventoryBox[j][i] = {};
                break;
            default:
                getItemAt(realTarget) = {};
        }
        return copy;
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

    void Inventory::layItem(const Item& item, int i, int j)
    {
        for (auto item_i = i; item_i < i + item.getInvSize().first; ++item_i)
            for (auto item_j = j; item_j < j + item.getInvSize().second; ++item_j)
            {
                auto& cell = mInventoryBox[item_j][item_i];
                cell = item;
                cell.mIsReal = false;
                cell.mCornerX = i;
                cell.mCornerY = j;
            }
        mInventoryBox[j][i].mIsReal = true;
    }

    bool Inventory::autoPlaceItem(const Item& item, boost::optional<std::pair<Inventory::xorder, Inventory::yorder>> override_order)
    {
        // auto-placing in belt
        if (item.isBeltEquippable())
            for (auto i = 0; i < beltWidth; ++i)
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
        boost::any_range<int, boost::single_pass_traversal_tag, int, std::ptrdiff_t> xrange, yrange;
        // that's a bit slow technique in general but I think it's fine
        if (requiredXOrder == xorder::fromLeft)
            xrange = boost::irange(0, inventoryWidth, 1);
        else
            xrange = boost::irange(inventoryWidth - 1, -1, -1);
        if (requiredYOrder == yorder::fromTop)
            yrange = boost::irange(0, inventoryHeight, 1);
        else
            yrange = boost::irange(inventoryHeight - 1, -1, -1);
        for (auto i : xrange)
        {
            if (i + item.getInvSize().first > inventoryWidth)
                continue;
            for (auto j : yrange)
            {
                if (j + item.getInvSize().second > inventoryHeight)
                    continue;

                if (![&] {
                        for (auto item_i = i; item_i < i + item.getInvSize().first; ++item_i)
                            for (auto item_j = j; item_j < j + item.getInvSize().second; ++item_j)
                                if (!mInventoryBox[item_j][item_i].isEmpty())
                                    return false;
                        return true;
                    }())
                    continue;

                layItem(item, i, j);
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
        int beltX = static_cast<int>(x * beltWidth);
        exchangeWithCursor(MakeEquipTarget<EquipTargetType::belt>(beltX));
    }

    void Inventory::inventoryMouseLeftButtonDown(double x, double y)
    {
        int takeoutCellX = static_cast<int>(x * inventoryWidth);
        int takeoutCellY = static_cast<int>(y * inventoryHeight);
        int placementCellX = static_cast<int>(x * inventoryWidth - mCursorHeld.getInvSize().first * 0.5 + 0.5);
        int placementCellY = static_cast<int>(y * inventoryHeight - mCursorHeld.getInvSize().second * 0.5 + 0.5);
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
                return mInventoryBox[target.posY][target.posX];
            case EquipTargetType::belt:
                return mBelt[target.posX];
            case EquipTargetType::cursor:
                return mCursorHeld;
            default:
                break;
        }

        return Item::empty;
    }

    bool Inventory::fitsAt(Item item, uint8_t x, uint8_t y)
    {
        bool foundItem = false;

        if (y + item.mSizeY < 5 && x + item.mSizeX < 11)
        {
            for (uint8_t k = y; k < y + item.mSizeY; k++)
            {
                for (uint8_t l = x; l < x + item.mSizeX; l++)
                {
                    if (foundItem)
                        break;
                    if (!mInventoryBox[k][l].isEmpty())
                    {
                        foundItem = true;
                    }
                    if (l == (x + item.mSizeX - 1) && k == (y + item.mSizeY - 1) && !foundItem)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void Inventory::dump()
    {
        std::stringstream ss;
        for (uint8_t i = 0; i < 4; i++)
        {
            for (uint8_t j = 0; j < 10; j++)
            {
                if (mInventoryBox[i][j].isEmpty())
                    ss << "| (empty)";
                else
                    ss << "| " << mInventoryBox[i][j].getName();
                if (mInventoryBox[i][j].mCount > 1)
                    ss << "(" << +mInventoryBox[i][j].mCount << ")";
                ss << "   ";
            }
            ss << " |" << std::endl;
        }

        size_t len = ss.str().length() / 4;
        std::string tops = "";
        tops.append(len, '-');
        std::cout << tops << std::endl << ss.str() << tops << std::endl;
        std::cout << "head: " << mHead.getName() << std::endl;
        std::cout << "mBody: " << mBody.getName() << std::endl;
        std::cout << "mAmulet: " << mAmulet.getName() << std::endl;
        std::cout << "mRightHand: " << mRightHand.getName() << std::endl;
        std::cout << "mLeftHand: " << mLeftHand.getName() << std::endl;
        std::cout << "mLeftRing: " << mLeftRing.getName() << std::endl;
        std::cout << "mRightRing: " << mRightRing.getName() << std::endl;
        std::cout << "mCursorHeld: " << mCursorHeld.getName() << std::endl;
        std::stringstream printbelt;
        printbelt << "mBelt: ";
        for (size_t i = 0; i < 8; i++)
        {
            printbelt << mBelt[i].getName() << ", ";
        }
        std::cout << printbelt.str() << std::endl;
    }
}
