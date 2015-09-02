#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include "inventory.h"
#include "itemmanager.h"
#include "player.h"
#include "actorstats.h"
#include <algorithm>
namespace FAWorld
{
    Inventory::Inventory(Actor * actor)
    {
        mAttackDamageTotal=0;
        mArmourClassTotal=0;
        mActor = actor;
        for(uint8_t i=0;i<4;i++)
        {

            for(uint8_t j=0;j<10;j++)
            {
                mInventoryBox[i][j].mInvX=j;
                mInventoryBox[i][j].mInvY=i;

            }
        }
    }

    uint32_t Inventory::getTotalArmourClass()
    {
        return mArmourClassTotal;
        if(mActor == NULL)
            return 0;
    }

    uint32_t Inventory::getTotalAttackDamage()
    {

        return mAttackDamageTotal;
    }

    bool Inventory::checkStatsRequirement(Item& item)
    {

        if(mActor->mStats != NULL)
            if(!(mActor->mStats->getStrength() >= item.getReqStr() &&
                 mActor->mStats->getDexterity()>= item.getReqDex() &&
                 mActor->mStats->getMagic() >= item.getReqMagic()  &&
                 mActor->mStats->getVitality() >= item.getReqVit()))
                return false;
            else
                return true;
        else
            return true;
    }

    void Inventory::setPlayerFormat()
    {
        std::string armour, weapon;


        switch(mBody.getCode())
        {
               case Item::icHeavyArmour:
                armour="h";
               break;

               case Item::icMidArmour:
                armour="m";
               break;

               case Item::icLightArmour:
               default:
                armour="l";
               break;
        }

        if(mLeftHand.isEmpty() && mRightHand.isEmpty())
        {
            weapon = "n";
        }
        else if((mLeftHand.isEmpty() && !mRightHand.isEmpty()) || (!mLeftHand.isEmpty() && mRightHand.isEmpty()))
        {
            Item hand;

            if(mRightHand.isEmpty())
                hand = mLeftHand;
            else
                hand = mRightHand;

            switch(hand.getCode())
            {
                case Item::icAxe:
                    if(hand.getEquipLoc() == Item::eqONEHAND)
                        weapon = "s";
                    else
                        weapon = "a";
                    break;

                case Item::icBlunt:
                    weapon = "m";
                    break;

                case Item::icBow:
                    weapon = "b";
                    break;

                case Item::icShield:
                    weapon = "u";
                    break;

                case Item::icSword:
                    weapon = "s";
                    break;

                default:                    
                    weapon = "n";
                    break;
            }
            printf("after switch: %s, %d\n", weapon.c_str(), hand.getCode() );
        }

        else if(!mLeftHand.isEmpty() && !mRightHand.isEmpty())
        {            
            if((mLeftHand.getCode() == Item::icSword && mRightHand.getCode() == Item::icShield) || (mLeftHand.getCode() == Item::icShield && mRightHand.getCode() == Item::icSword))
                weapon = "d";

            else if(mLeftHand.getCode() == Item::icBow && mRightHand.getCode() == Item::icBow)
                weapon = "b";

            else if(mLeftHand.getCode() == Item::icStave && mRightHand.getCode() == Item::icStave)
                weapon = "t";
            else if(mLeftHand.getCode() == Item::icBlunt || mRightHand.getCode() == Item::icBlunt)
                weapon = "h";
        }

        mActor->mActorSpriteState.setWeapon(weapon);
        mActor->mActorSpriteState.setArmour(armour);


    }

    bool Inventory::canPlaceItem(
            Item  item,
            Item::equipLoc equipType,
            uint8_t y,
            uint8_t x,
            uint8_t beltX)
    {
        switch(equipType)
        {
        /* Characters in Diablo are ambidextrous so we can put a shield in the left
                 * or right hand and a sword in the left or right hand. You cannot duel
                 * wield swords nor can you carry two shields. Shields are classified as
                 * armour in the game.
                 *
                 * */
        case Item::eqLEFTHAND:
            if(!checkStatsRequirement(item))
                return false;

            if(item.getEquipLoc() == Item::eqONEHAND)
            {
                if(this->mLeftHand.isEmpty())
                {
                    if(this->mRightHand.isEmpty())
                        return true;

                    else if(this->mRightHand.getType() != item.getType())
                        return true;
                }
            }
            else if(item.getEquipLoc() == Item::eqTWOHAND)
            {
                if(item.getEquipLoc() == Item::eqTWOHAND && this->mLeftHand.isEmpty())
                    return true;

            }
            break;
        case Item::eqRIGHTHAND:
            if(!checkStatsRequirement(item))
                return false;
            if(this->mRightHand.isEmpty())
            {
                if(this->mLeftHand.isEmpty())
                    return true;

                else if(this->mLeftHand.getType() != item.getType())
                    return true;
            }

            else if(item.getEquipLoc() == Item::eqTWOHAND)
            {
                if(item.getEquipLoc() == Item::eqTWOHAND && this->mLeftHand.isEmpty())
                    return true;

            }
            break;
        case Item::eqBODY:
            if(!checkStatsRequirement(item))
                return false;
            if(item.getEquipLoc() == Item::eqBODY && this->mBody.isEmpty())
                return true;
            break;

        case Item::eqHEAD:
            if(!checkStatsRequirement(item))
                return false;
            if(item.getEquipLoc() == Item::eqHEAD && this->mHead.isEmpty())
                return true;
            break;

        case Item::eqLEFTRING:
            if(!checkStatsRequirement(item))
                return false;
            if(item.getEquipLoc() == Item::eqRING)
            {
                if(this->mLeftRing.isEmpty()) return true;
            }
            break;
        case Item::eqRIGHTRING:
            if(!checkStatsRequirement(item))
                return false;
            if(item.getEquipLoc() == Item::eqRING)
            {
                if(this->mRightRing.isEmpty()) return true;
            }
            break;

        case Item::eqAMULET:
            if(!checkStatsRequirement(item))
                return false;
            if(item.getEquipLoc() == Item::eqAMULET && this->mAmulet.isEmpty())
            {
                return true;
            }
            break;
            /*
                 * When putting an item in the inventory we must check if it will fit!
                 * */

        case Item::eqINV:
            if(x < 10 && y < 4)
            {
                if(!((x + item.mSizeX-1 < 10) && (y + item.mSizeY-1) < 4))
                {
                    return false;

                }
                if(mInventoryBox[y][x].isEmpty())
                {
                    for(uint8_t i=y;i < y+item.mSizeY; ++i)
                    {
                        for(uint8_t j=x; j < x+item.mSizeX;++j)
                        {
                            if(!mInventoryBox[i][j].isEmpty())
                            {
                                return false;
                            }
                        }
                    }
                    return true;
                }
            }
            break;
        case Item::eqBELT:
            if(item.getEquipLoc() == Item::eqUNEQUIP && item.getType() == Item::itPOT)
            {
                if(beltX <= 7)
                {
                    if(mBelt[beltX].isEmpty())
                    {
                        return true;
                    }
                }

            }
            break;
        case Item::eqFLOOR:
            return true;
        case Item::eqCURSOR:
            return this->mCursorHeld.isEmpty();
        default:
            return false;
        }
        return false;
    }
    //TODO: When stats have implemented add checks for requirements to wear/wield items
    bool Inventory::putItem(Item &item,
                            Item::equipLoc equipType,
                            Item::equipLoc from,
                            uint8_t y,
                            uint8_t x,
                            uint8_t beltX, bool recalculateStats)
    {

        if(canPlaceItem(item, equipType, y, x, beltX))
        {
            switch(equipType)
            {
            case Item::eqLEFTHAND:
                if(item.getEquipLoc() == Item::eqONEHAND)
                {
                    this->mLeftHand=item;

                    removeItem(item, from, beltX);
                }
                else if(item.getEquipLoc() == Item::eqTWOHAND)
                {

                    if(!this->mRightHand.isEmpty())
                    {
                        this->mLeftHand = item;
                        bool foundSpace=false;
                        uint8_t auto_fit_x=255, auto_fit_y=255;
                        for(uint8_t i=0;i<4;i++)
                        {
                            for(uint8_t j=0;j<10;j++)
                            {
                                if(foundSpace)
                                    break;
                                if(mInventoryBox[i][j].isEmpty())
                                {
                                    if(i+this->mRightHand.mSizeY < 5 && j+this->mRightHand.mSizeX < 11)
                                    {
                                        if(fitsAt(this->mRightHand,i,j))
                                        {
                                            auto_fit_x=j;
                                            auto_fit_y=i;
                                            foundSpace=true;
                                            break;
                                        }
                                    }
                                }
                                else
                                {
                                    continue;
                                }

                            }
                        }
                        if(auto_fit_x !=255)
                        {
                            putItem(this->mRightHand, Item::eqINV, Item::eqRIGHTHAND, auto_fit_y, auto_fit_x);
                            removeItem(item, from, beltX, y, x);
                            this->mRightHand = item;
                            if(recalculateStats)
                                collectEffects();
                            setPlayerFormat();
                            return true;
                        }
                        else
                        {
                            this->mCursorHeld = this->mRightHand;
                            this->mRightHand = this->mLeftHand;

                        }
                    }
                    else
                    {
                        this->mLeftHand = item;
                        this->mRightHand = item;
                        removeItem(item, from, beltX, y, x);

                    }

                }
                break;
            case Item::eqRIGHTHAND:
                this->mRightHand=item;
                if(item.getEquipLoc() == Item::eqTWOHAND)
                {
                    if(this->mLeftHand.isEmpty())
                    {
                        this->mLeftHand=item;

                    }
                    else
                    {
                        return putItem(item, Item::eqLEFTHAND, from);

                    }

                }
                removeItem(item, from, beltX);
                break;
                /*
                     * When wielding a two-handed weapon the game will unload the right
                     * hand weapon if there is one. If there is space in your inventory
                     * it will automatically put it there. If there is not it will give
                     * it to you to sort out for yourself.
                     * */

            case Item::eqBODY:
                mBody = item;
                removeItem(item, from, beltX);
                break;

            case Item::eqHEAD:
                mHead = item;
                removeItem(item, from, beltX);
                break;

            case Item::eqLEFTRING:
                mLeftRing = item;
                removeItem(item, from, beltX);
                break;
            case Item::eqRIGHTRING:
                mRightRing = item;
                removeItem(item, from, beltX);
                break;

            case Item::eqAMULET:
                mAmulet = item;
                removeItem(item, from, beltX);
                break;

                /*
                     * For every space the item takes up in the inventory we leave
                     * a reference to the item in the corresponding entry in mInventoryBox.
                    */
            case Item::eqINV:

                if(item.mMaxCount > 1)
                {
                    for(uint8_t i=0;i<4;i++)
                    {
                        for(uint8_t j=0;j<10;j++)
                        {
                            if((mInventoryBox[i][j].mBaseId == item.mBaseId) && (mInventoryBox[i][j].mCount+item.mCount) <=item.mMaxCount && (mInventoryBox[i][j].mUniqueId != item.mUniqueId))
                            {
                                mInventoryBox[i][j].mCount+=item.mCount;

                                if(mInventoryBox[i][j].mCount < 1000)
                                    mInventoryBox[i][j].mGraphicValue=GOLD_PILE_MIN;

                                else if (mInventoryBox[i][j].mCount > 1000 && mInventoryBox[i][j].mCount < 2500)
                                    mInventoryBox[i][j].mGraphicValue=GOLD_PILE_MID;

                                else
                                    mInventoryBox[i][j].mGraphicValue=GOLD_PILE_MAX;
                                removeItem(item, from, item.mInvX, item.mInvY, item.mBeltX);

                                if(recalculateStats)
                                    collectEffects();
                                setPlayerFormat();
                                return true;
                            }
                            else if(mInventoryBox[i][j] == item)
                            {
                                item.mInvX = x;
                                item.mInvY = y;
                                mInventoryBox[y][x] = item;
                                mInventoryBox[i][j] = Item();
                                break;
                            }
                        }
                    }
                }

                for(uint8_t i=y;i<y+item.mSizeY;i++)
                {
                    for(uint8_t j=x;j<x+item.mSizeX;j++)
                    {

                        mInventoryBox[i][j] = item;
                        if(i==y && j==x)
                        {
                            mInventoryBox[i][j].mInvX=j;
                            mInventoryBox[i][j].mInvY=i;
                            mInventoryBox[i][j].mCornerX=x;
                            mInventoryBox[i][j].mCornerY=y;
                            mInventoryBox[i][j].mIsReal=true;
                        }
                        else
                        {
                            mInventoryBox[i][j].mInvX=j;
                            mInventoryBox[i][j].mInvY=i;
                            mInventoryBox[i][j].mCornerX=x;
                            mInventoryBox[i][j].mCornerY=y;
                            mInventoryBox[i][j].mIsReal=false;
                        }
                    }
                }
                removeItem(item, from, beltX, y, x);
                break;
            case Item::eqBELT:
                item.mBeltX = beltX;
                removeItem(item, from, beltX);
                this->mBelt[beltX]=item;
                break;
            case Item::eqFLOOR:
                break;
            case Item::eqCURSOR:

                mCursorHeld = item;


                removeItem(item, from, beltX, y, x);
                break;
            default:
                return false;

            }
        }
        else
        {
            return false;
        }
        if(recalculateStats)
            collectEffects();
        setPlayerFormat();
        return true;
    }

    Item& Inventory::getItemAt(Item::equipLoc type, uint8_t y, uint8_t x, uint8_t beltX)
    {


        switch(type)
        {
        case Item::eqLEFTHAND: return mLeftHand;
        case Item::eqLEFTRING: return mLeftRing;
        case Item::eqRIGHTHAND: return mRightHand;
        case Item::eqRIGHTRING: return mRightRing;
        case Item::eqBODY: return mBody;
        case Item::eqHEAD: return mHead;
        case Item::eqAMULET: return mAmulet;
        case Item::eqINV: return mInventoryBox[y][x];
        case Item::eqBELT: return mBelt[beltX];
        case Item::eqCURSOR: return mCursorHeld;
        default: break;

        }

        return Item::empty;
    }

    void Inventory::removeItem(
            Item& item,
            Item::equipLoc from,
            uint8_t beltX,
            uint8_t invY,
            uint8_t invX)
    {
        switch(from)
        {
        case Item::eqLEFTHAND:
            if(item.getEquipLoc() == Item::eqTWOHAND)
            {
                mRightHand=Item();
            }
            mLeftHand = Item();
            break;

        case Item::eqRIGHTHAND:
            if(item.getEquipLoc() == Item::eqTWOHAND)
            {

                mLeftHand=Item();
            }
            mRightHand=Item();
            break;

        case Item::eqLEFTRING:
            mLeftRing=Item();
            break;
        case Item::eqRIGHTRING:
            mRightRing =Item();
            break;

        case Item::eqBELT:
            mBelt[beltX] =Item();
            break;

        case Item::eqCURSOR:
            mCursorHeld = Item();
            break;

        case Item::eqAMULET:
            mAmulet = Item();
            break;

        case Item::eqHEAD:
            mHead = Item();
            break;

        case Item::eqBODY:
            mBody = Item();
            break;

        case Item::eqINV:
            for(uint8_t i=invY;i < invY+item.mSizeY;i++)
            {
                for(uint8_t j=invX;j < invX+item.mSizeX;j++)
                {
                    mInventoryBox[i][j] = Item();
                    mInventoryBox[i][j].mInvY=i;
                    mInventoryBox[i][j].mInvX=j;
                }
            }
            break;

        case Item::eqONEHAND:
        case Item::eqTWOHAND:
        case Item::eqUNEQUIP:
        case Item::eqFLOOR:
        default:
            return;
        }

    }
    bool Inventory::fitsAt(Item item, uint8_t y, uint8_t x)
    {
        bool foundItem=false;

        if(y+item.mSizeY < 5 && x+item.mSizeX < 11)
        {
            for(uint8_t k=y;k<y+item.mSizeY;k++)
            {
                for(uint8_t l=x;l<x+item.mSizeX;l++)
                {
                    if(foundItem)
                        break;
                    if(!mInventoryBox[k][l].isEmpty())
                    {
                        foundItem = true;
                    }
                    if(l==(x + item.mSizeX-1) &&
                            k==(y + item.mSizeY-1) &&
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
        for(uint8_t i=0;i<4;i++)
        {
            for(uint8_t j=0;j<10;j++)
            {
                if(mInventoryBox[i][j].isEmpty())
                    ss << "| (empty)";
                else
                    ss << "| "<< mInventoryBox[i][j].getName();
                if(mInventoryBox[i][j].mCount > 1)
                    ss << "("<<+mInventoryBox[i][j].mCount << ")";
                ss << "   ";

            }
            ss << " |" << std::endl;



        }

        size_t len = ss.str().length()/4;
        std::string tops = "";
        tops.append(len,'-');
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
        printbelt <<  "mBelt: ";
        for(size_t i=0; i<8; i++)
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
            mArmourClassTotal=0;
            mAttackDamageTotal=0;
            if(!mHead.isEmpty())
            {
                mArmourClassTotal += mHead.mArmourClass;
                mAttackDamageTotal+= mHead.mAttackDamage;
            }

            if(!mBody.isEmpty())
            {
                mArmourClassTotal += mBody.mArmourClass;
                mAttackDamageTotal+= mBody.mAttackDamage;
            }

            if(!mAmulet.isEmpty())
            {
                mArmourClassTotal += mAmulet.mArmourClass;
                mAttackDamageTotal+= mAmulet.mAttackDamage;
            }

            if(!mRightRing.isEmpty())
            {
                mArmourClassTotal += mRightRing.mArmourClass;
                mAttackDamageTotal+= mRightRing.mAttackDamage;
            }

            if(!mLeftRing.isEmpty())
            {
                mArmourClassTotal += mLeftRing.mArmourClass;
                mAttackDamageTotal+= mLeftRing.mAttackDamage;
            }
            if(!mLeftHand.isEmpty())
            {
                mArmourClassTotal += mLeftHand.mArmourClass;
                mAttackDamageTotal+= mLeftHand.mAttackDamage;
            }
            mItemEffects.insert(mItemEffects.end(), mHead.mEffects.begin(), mHead.mEffects.end());
            mItemEffects.insert(mItemEffects.end(), mBody.mEffects.begin(), mBody.mEffects.end());
            mItemEffects.insert(mItemEffects.end(), mAmulet.mEffects.begin(), mAmulet.mEffects.end());
            mItemEffects.insert(mItemEffects.end(), mRightRing.mEffects.begin(), mRightRing.mEffects.end());
            mItemEffects.insert(mItemEffects.end(), mLeftRing.mEffects.begin(), mLeftRing.mEffects.end());
            mItemEffects.insert(mItemEffects.end(), mLeftHand.mEffects.begin(), mLeftHand.mEffects.end());
            if(!(mLeftHand == mRightHand))
            {
                mItemEffects.insert(mItemEffects.end(), mRightHand.mEffects.begin(), mRightHand.mEffects.end());
                if(!mRightHand.isEmpty())
                {
                    mArmourClassTotal += mRightHand.mArmourClass;
                    mAttackDamageTotal+= mRightHand.mAttackDamage;
                }
            }
            if(mActor->mStats != NULL)
                mActor->mStats->recalculateDerivedStats();;
        }

    std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> &Inventory::getTotalEffects()
    {

        return mItemEffects;
    }
}
