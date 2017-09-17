#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include "inventory.h"
#include "itemmanager.h"
#include "player.h"
#include "actorstats.h"
#include <algorithm>
#include <boost/range/irange.hpp>
#pragma warning(push)
#pragma warning(disable:4172)  // boost being naughty
#include <boost/range/any_range.hpp>
#pragma warning(pop)

namespace FAWorld
{
    Inventory::Inventory(Player* actor)
    {
        mAttackDamageTotal = 0;
        mArmourClassTotal = 0;
        mActor = actor;
        for(uint8_t i = 0; i < 4; i++)
        {
            for(uint8_t j = 0; j < 10; j++)
            {
                mInventoryBox[i][j].mInvX = j;
                mInventoryBox[i][j].mInvY = i;
            }
        }
    }

    uint32_t Inventory::getTotalArmourClass()
    {
        if(mActor == NULL)
            return 0;
        return mArmourClassTotal;
    }

    uint32_t Inventory::getTotalAttackDamage()
    {
        return mAttackDamageTotal;
    }

    bool Inventory::checkStatsRequirement(const Item& item) const
    {
        if(mActor->mStats != NULL)
            if(!(mActor->mStats->getStrength() >= item.getReqStr() &&
                 mActor->mStats->getDexterity() >= item.getReqDex() &&
                 mActor->mStats->getMagic() >= item.getReqMagic() &&
                 mActor->mStats->getVitality() >= item.getReqVit()))
                return false;
            else
                return true;
        else
            return true;
    }

    static const std::map <Item::equipLoc, std::set <Item::equipLoc>> appropriateLocations =
    {
        {Item::eqONEHAND, {Item::eqLEFTHAND, Item::eqRIGHTHAND}},
        {Item::eqTWOHAND, {Item::eqLEFTHAND, Item::eqRIGHTHAND}},
        {Item::eqRING, {Item::eqLEFTRING, Item::eqRIGHTRING}},
        {Item::eqAMULET, {Item::eqAMULET}},
        {Item::eqBODY, {Item::eqBODY}},
        {Item::eqHEAD, {Item::eqHEAD}},
    };

    bool Inventory::isFit(const Item& item, const EquipTarget& target) const
    {
        switch(target.location)
        {
            case Item::eqINV:
                return target.posX + item.getInvSize().first <= inventoryWidth && target.posY + item.getInvSize().second <= inventoryHeight;
            case Item::eqBELT:
                return item.isBeltEquippable ();
            default:
            {
                auto it = appropriateLocations.find(item.getEquipLoc());
                if(it != appropriateLocations.end())
                    return it->second.count(target.location) > 0;
                break;
            }
        }
        return false;
    }

    auto Inventory::needsToBeExchanged(const Item& item, const EquipTarget& target) const -> ExchangeResult
    {
        switch(target.location)
        {
            case Item::eqLEFTHAND:
            case Item::eqRIGHTHAND:
            {
                auto& thisHand = getItemAt(target);
                auto getOtherHand = [](const EquipTarget& target) -> EquipTarget
                {
                    if(target.location == Item::eqLEFTHAND)
                        return MakeEquipTarget <Item::eqRIGHTHAND>();
                    else
                        return MakeEquipTarget <Item::eqLEFTHAND>();
                };
                auto& otherHand = getItemAt(getOtherHand(target));
                if(thisHand.getEquipLoc() == Item::eqTWOHAND)
                    return {{MakeEquipTarget <Item::eqLEFTHAND>()}, {}};
                if(otherHand.isEmpty())
                {
                    if(thisHand.isEmpty())
                        return {{}, {}};
                    else
                        return {{target}, {}};
                }
                // case where weapon and shield are equipped
                auto checkHand = [&](const EquipTarget& hand) -> boost::optional <ExchangeResult>
                {
                    auto& handItem = getItemAt(hand);
                    auto& otherHandItem = getItemAt(getOtherHand(hand));
                    if(item.getEquipLoc() == Item::eqTWOHAND)
                    {
                        // in this case we need to exchange with weapon and place shield back to inventory if possible
                        // if it's not possible then this item equipping should also be deemed impossible.
                        if(otherHandItem.isEmpty())
                            return ExchangeResult{{hand}, {}};
                        else if(handItem.getType() == Item::itWEAPON)
                            return ExchangeResult{{hand}, {getOtherHand(hand)}};
                    }
                    if(handItem.getType() == item.getType())
                    {
                        // if it's shield, it is replaced with shield, if it's 1h weapon, it's replaced with it
                        // no matter which slot we clicked
                        return ExchangeResult{{hand}, {}, hand};
                    }
                    return {};
                };
                if(auto res = checkHand(MakeEquipTarget <Item::eqLEFTHAND>())) return *res;
                if(auto res = checkHand(MakeEquipTarget <Item::eqRIGHTHAND>())) return *res;
                break;
            }
            case Item::eqINV:
            {
                ExchangeResult result;
                for(auto i = target.posX; i < target.posX + item.getInvSize().first; ++i)
                    for(auto j = target.posY; j < target.posY + item.getInvSize().second; ++j)
                        if(!mInventoryBox[j][i].isEmpty())
                        {
                            auto cornerCoords = mInventoryBox[j][i].getCornerCoords();
                            result.NeedsToBeReplaced.insert(MakeEquipTarget <Item::eqINV>(cornerCoords.first, cornerCoords.second));
                        }
                return result;
            }
            default:
                if(!getItemAt(target).isEmpty())
                    return {{target}, {}};
        }
        return {{}, {}};
    }

    EquipTarget Inventory::avoidLinks(const EquipTarget& target)
    {
        switch(target.location)
        {
            case Item::eqINV:
            {
                auto& item = getItemAt(target);
                return MakeEquipTarget <Item::eqINV>(item.getCornerCoords().first, item.getCornerCoords().second);
            }
            default:
                return target;
        }
    }

    Item Inventory::takeOut(const EquipTarget& target)
    {
        auto realTarget = avoidLinks(target);
        auto copy = getItemAt(realTarget);
        if(copy.getEquipLoc() == Item::eqTWOHAND)
        {
            if(target.location == Item::eqLEFTHAND)
                getItemAt(MakeEquipTarget <Item::eqRIGHTHAND>()) = {};
            else if(target.location == Item::eqRIGHTHAND)
                return {}; // Cancel the operation, you can't click on illusionary two-hand item on the right in diablo
        }
        switch(target.location)
        {
            case Item::eqINV:
                for(int j = realTarget.posY; j < realTarget.posY + copy.getInvSize().second; ++j)
                    for(int i = realTarget.posX; i < realTarget.posX + copy.getInvSize().first; ++i)
                        mInventoryBox[j][i] = {};
                break;
            default:
                getItemAt(realTarget) = {};
        }
        return copy;
    }


    std::set <Item::equipLoc> wearable = {
        Item::eqLEFTRING, Item::eqRIGHTRING, Item::eqAMULET, Item::eqLEFTHAND, Item::eqRIGHTHAND, Item::eqBODY, Item::eqHEAD
    };

    void Inventory::putItemUnsafe(const Item& item, const EquipTarget& target)
    {
        if(item.getEquipLoc() == Item::eqTWOHAND && target.location == Item::eqLEFTHAND)
        {
            auto& rightHand = getItemAt(MakeEquipTarget <Item::eqRIGHTHAND>());
            rightHand = item;
            rightHand.mIsReal = false;
        }
        switch(target.location)
        {
            default:
                getItemAt(target) = item;
                break;
            case Item::eqINV:
                layItem(item, target.posX, target.posY);
                break;
        }
    }

    void Inventory::layItem(const Item& item, int i, int j)
    {
        for(auto item_i = i; item_i < i + item.getInvSize().first; ++item_i)
            for(auto item_j = j; item_j < j + item.getInvSize().second; ++item_j)
            {
                auto& cell = mInventoryBox[item_j][item_i];
                cell = item;
                cell.mIsReal = false;
                cell.mCornerX = i;
                cell.mCornerY = j;
            }
        mInventoryBox[j][i].mIsReal = true;
    }

    bool Inventory::autoPlaceItem(const Item& item, boost::optional <std::pair <Inventory::xorder, Inventory::yorder>> override_order)
    {
        // auto-placing in belt
        if(item.isBeltEquippable())
            for(auto i = 0; i < beltWidth; ++i)
            {
                auto& place = getItemAt(MakeEquipTarget <Item::eqBELT>(i));
                if(place.isEmpty())
                {
                    place = item;
                    return true;
                }
            }
        // auto-equipping weapons
        auto& leftHand = getItemAt(MakeEquipTarget <Item::eqLEFTHAND>());
        auto& rightHand = getItemAt(MakeEquipTarget <Item::eqRIGHTHAND>());
        if(item.getEquipLoc() == Item::eqTWOHAND && leftHand.isEmpty() && rightHand.isEmpty())
        {
            putItemUnsafe(item, MakeEquipTarget <Item::eqLEFTHAND>());
        }
        // only for weapons, not shields
        if(item.getEquipLoc() == Item::eqONEHAND && item.getType() == Item::itWEAPON)
            for(auto hand_ptr : {&leftHand, &rightHand})
                if(hand_ptr->isEmpty())
                {
                    *hand_ptr = item;
                    return true;
                }

        // different orders of placement for different item types as found in original game:
        auto requiredXOrder = xorder::fromLeft;
        auto requiredYOrder = yorder::fromTop;
        switch(item.getType())
        {
            case Item::itARMOUR:
                if(item.getEquipLoc() == Item::eqONEHAND)
                    requiredXOrder = xorder::fromRight;
                break;
            case Item::itPOT: // TODO: scrolls
                requiredYOrder = yorder::fromBottom;
                break;
            case Item::itGOLD:
                requiredYOrder = yorder::fromBottom;
                requiredXOrder = xorder::fromRight;
                break;
            default:
                break;
        }
        if(override_order)
        {
            requiredXOrder = override_order->first;
            requiredYOrder = override_order->second;
        }
        boost::any_range <int, boost::single_pass_traversal_tag, int, std::ptrdiff_t> xrange, yrange;
        // that's a bit slow technique in general but I think it's fine
        if(requiredXOrder == xorder::fromLeft) xrange = boost::irange(0, inventoryWidth, 1);
        else xrange = boost::irange(inventoryWidth - 1, -1, -1);
        if(requiredYOrder == yorder::fromTop) yrange = boost::irange(0, inventoryHeight, 1);
        else yrange = boost::irange(inventoryHeight - 1, -1, -1);
        for(auto i : xrange)
            {
                if(i + item.getInvSize().first > inventoryWidth)
                   continue;
                for(auto j : yrange)
                {
                    if(j + item.getInvSize().second > inventoryHeight)
                        continue;

                    if(![&]
                    {
                        for(auto item_i = i; item_i < i + item.getInvSize().first; ++item_i)
                            for(auto item_j = j; item_j < j + item.getInvSize().second; ++item_j)
                                if(!mInventoryBox[item_j][item_i].isEmpty())
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

    bool Inventory::exchangeWithCursor(EquipTarget takeoutTarget, boost::optional <EquipTarget> maybePlacementTarget)
    {
        auto placementTarget = maybePlacementTarget ? *maybePlacementTarget : takeoutTarget;
        if(mCursorHeld.isEmpty())
        {
            if(getItemAt(takeoutTarget).isEmpty())
                return false;

            setCursorHeld(takeOut(takeoutTarget));
            return true;
        }

        auto& item = mCursorHeld;
        if(item.getEquipLoc() == Item::eqTWOHAND && placementTarget.location == Item::eqRIGHTHAND)
            placementTarget = MakeEquipTarget <Item::eqLEFTHAND>();

        if(wearable.count(placementTarget.location) > 0 && !checkStatsRequirement(item))
            return false;

        if(!isFit(item, placementTarget))
            return false;

        auto requirements = needsToBeExchanged(item, placementTarget);
        if(requirements.NeedsToBeReplaced.size() > 1)
            return false;

        for(auto& location : requirements.NeedsToBeReturned)
        {
            // yes this is insanity but in this case order of auto placement differs from usual
            if(!autoPlaceItem(getItemAt(location), std::make_pair(xorder::fromLeft, yorder::fromTop)))
                return false;

            takeOut(location); // take out and discard
        }

        placementTarget = requirements.newTarget ? *requirements.newTarget : placementTarget;

        if(requirements.NeedsToBeReplaced.empty())
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

    void Inventory::itemSlotLeftMouseButtonDown(EquipTarget target)
    {
        exchangeWithCursor(target);
    }

    void Inventory::beltMouseLeftButtonDown(double x)
    {
        int beltX = static_cast <int>(x * beltWidth);
        exchangeWithCursor(MakeEquipTarget <Item::eqBELT>(beltX));
    }

    void Inventory::inventoryMouseLeftButtonDown(double x, double y)
    {
        int takeoutCellX = static_cast <int>(x * inventoryWidth);
        int takeoutCellY = static_cast <int>(y * inventoryHeight);
        int placementCellX = static_cast <int>(x * inventoryWidth - mCursorHeld.getInvSize().first * 0.5 + 0.5);
        int placementCellY = static_cast <int>(y * inventoryHeight - mCursorHeld.getInvSize().second * 0.5 + 0.5);
        if(!isValidCell(takeoutCellX, takeoutCellY))
            return;
        exchangeWithCursor(MakeEquipTarget <Item::eqINV>(takeoutCellX, takeoutCellY),
                           MakeEquipTarget <Item::eqINV>(placementCellX, placementCellY));
    }

    void Inventory::setCursorHeld(const Item& item)
    {
        mCursorHeld = item;
        updateCursor();
    }

    void Inventory::updateCursor()
    {
        if(mCursorHeld.isEmpty())
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

    const Item& Inventory::getItemAt(const EquipTarget& target) const
    {
        return const_cast <Inventory *>(this)->getItemAt(target);
    }

    Item& Inventory::getItemAt(const EquipTarget& target)
    {
        switch(target.location)
        {
            case Item::eqLEFTHAND:
                return mLeftHand;
            case Item::eqLEFTRING:
                return mLeftRing;
            case Item::eqRIGHTHAND:
                return mRightHand;
            case Item::eqRIGHTRING:
                return mRightRing;
            case Item::eqBODY:
                return mBody;
            case Item::eqHEAD:
                return mHead;
            case Item::eqAMULET:
                return mAmulet;
            case Item::eqINV:
                return mInventoryBox[target.posY][target.posX];
            case Item::eqBELT:
                return mBelt[target.posX];
            case Item::eqCURSOR:
                return mCursorHeld;
            default:
                break;
        }

        return Item::empty;
    }

    void Inventory::removeItem(
        Item& item,
        Item::equipLoc from,
        uint8_t beltX,
        uint8_t invX,
        uint8_t invY)
    {
        switch(from)
        {
            case Item::eqLEFTHAND:
            {
                if(item.getEquipLoc() == Item::eqTWOHAND)
                {
                    mRightHand = Item();
                }
                mLeftHand = Item();
                break;
            }

            case Item::eqRIGHTHAND:
            {
                if(item.getEquipLoc() == Item::eqTWOHAND)
                {
                    mLeftHand = Item();
                }
                mRightHand = Item();
                break;
            }

            case Item::eqLEFTRING:
            {
                mLeftRing = Item();
                break;
            }
            case Item::eqRIGHTRING:
            {
                mRightRing = Item();
                break;
            }

            case Item::eqBELT:
            {
                mBelt[beltX] = Item();
                break;
            }

            case Item::eqCURSOR:
            {
                setCursorHeld({});
                break;
            }

            case Item::eqAMULET:
            {
                mAmulet = Item();
                break;
            }

            case Item::eqHEAD:
            {
                mHead = Item();
                break;
            }

            case Item::eqBODY:
            {
                mBody = Item();
                break;
            }

            case Item::eqINV:
            {
                // TODO: refactor everything
                auto sizeX = item.mSizeX;
                auto sizeY = item.mSizeY;
                for(uint8_t i = invY; i < invY + sizeY; i++)
                {
                    for(uint8_t j = invX; j < invX + sizeX; j++)
                    {
                        mInventoryBox[i][j] = Item();
                        mInventoryBox[i][j].mInvY = i;
                        mInventoryBox[i][j].mInvX = j;
                    }
                }
                break;
            }

            case Item::eqONEHAND:
            case Item::eqTWOHAND:
            case Item::eqUNEQUIP:
            case Item::eqFLOOR:
            default:
            {
                return;
            }
        }
    }

    bool Inventory::fitsAt(Item item, uint8_t x, uint8_t y)
    {
        bool foundItem = false;

        if(y + item.mSizeY < 5 && x + item.mSizeX < 11)
        {
            for(uint8_t k = y; k < y + item.mSizeY; k++)
            {
                for(uint8_t l = x; l < x + item.mSizeX; l++)
                {
                    if(foundItem)
                        break;
                    if(!mInventoryBox[k][l].isEmpty())
                    {
                        foundItem = true;
                    }
                    if(l == (x + item.mSizeX - 1) &&
                       k == (y + item.mSizeY - 1) &&
                       !foundItem)
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
        for(uint8_t i = 0; i < 4; i++)
        {
            for(uint8_t j = 0; j < 10; j++)
            {
                if(mInventoryBox[i][j].isEmpty())
                    ss << "| (empty)";
                else
                    ss << "| " << mInventoryBox[i][j].getName();
                if(mInventoryBox[i][j].mCount > 1)
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
        for(size_t i = 0; i < 8; i++)
        {
            printbelt << mBelt[i].getName() << ", ";
        }
        std::cout << printbelt.str() << std::endl;
    }

    void Inventory::collectEffects()
    {
        if(mActor == NULL)
            return;
        mItemEffects.clear();
        mArmourClassTotal = 0;
        mAttackDamageTotal = 0;

        auto addEffectsAndStats = [this](const Item& item)
        {
            mItemEffects.insert(mItemEffects.end(), item.mEffects.begin(), item.mEffects.end());
            if(!item.isEmpty())
            {
                mArmourClassTotal += item.mArmourClass;
                mAttackDamageTotal += item.mAttackDamage;
            }
        };

        for(auto item_ptr : {&mHead, &mBody, &mAmulet, &mRightRing, &mLeftRing, &mLeftHand})
            addEffectsAndStats(*item_ptr);
        if(!(mLeftHand == mRightHand))
        {
            addEffectsAndStats(mRightHand);
        }
        if(mActor->mStats != NULL)
            mActor->mStats->recalculateDerivedStats();
    }

    std::vector <std::tuple <Item::ItemEffect, uint32_t, uint32_t, uint32_t>>& Inventory::getTotalEffects()
    {
        return mItemEffects;
    }
}
