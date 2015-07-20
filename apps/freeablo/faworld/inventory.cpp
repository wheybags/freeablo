#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include "inventory.h"
#include <level/itemmanager.h>
namespace FAWorld
{
Inventory::Inventory()
{


}
Inventory::~Inventory()
{
}

Inventory *Inventory::testInv()
{


    Inventory * inv = new Inventory;
    Level::ItemManager itemManager;
    while(!itemManager.mIsLoaded);
    Level::Item gold = itemManager.getBaseItem(39);

    //gold.setCount(10);
    Level::Item bow  = itemManager.getBaseItem(24);
    Level::Item buckler = itemManager.getBaseItem(16);
    Level::Item ring = itemManager.getBaseItem(32);
    inv->putItem(itemManager.getBaseItem(24), Level::Item::eqINV, Level::Item::eqFLOOR);
    //inv->putItem(gold, Level::Item::eqINV, Level::Item::eqFLOOR, 0, 0);
    //inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 0);
    //inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 1);
    //inv->putItem(gold, Level::Item::eqINV, Level::Item::eqFLOOR, 0, 1);
//    std::cout << mInventoryBox[0][0].mItem.itemName << std::endl;
    //inv.putItem(gold, Level::Item::eqINV, Level::Item::eqINV, 0, 1);
//    std::cout << mInventoryBox[0][1].getName() <<'\0' << std::endl;
/*
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 2);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 3);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 4);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 5);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 6);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 7);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 8);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 0, 9);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 0);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 1);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 2);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 3);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 4);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 5);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 6);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 7);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 8);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 1, 9);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 0);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 1);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 2);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 3);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 4);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 5);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 6);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 7);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 8);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 2, 9);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 0);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 1);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 2);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 3);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 4);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 5);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 6);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 7);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 8);
    inv->putItem(itemManager.getBaseItem(32), Level::Item::eqINV, Level::Item::eqFLOOR, 3, 9);*/
    //inv->putItem(buckler, Level::Item::eqRIGHTHAND, Level::Item::eqFLOOR, 0, 0, 0);
    //inv->putItem(ring, Level::Item::eqRIGHTRING, Level::Item::eqFLOOR, 1, 1);
    //inv->putItem(bow, Level::Item::eqLEFTHAND, Level::Item::eqFLOOR, 1, 1);

    inv->dump();

    return inv;

}

bool Inventory::canPlaceItem(
        Level::Item  item,
        Level::Item::equipLoc equipType,
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
    case Level::Item::eqLEFTHAND:
        if(item.mItem.equipLoc == Level::Item::eqONEHAND)
        {
            if(this->mLeftHand.isEmpty())
            {
                if(this->mRightHand.isEmpty())
                    return true;

                else if(this->mRightHand.mItem.itemType != item.mItem.itemType)
                    return true;
            }
        }
        else if(item.mItem.equipLoc == Level::Item::eqTWOHAND)
        {
            if(item.mItem.equipLoc == Level::Item::eqTWOHAND && this->mLeftHand.isEmpty())
                return true;

        }
        break;
    case Level::Item::eqRIGHTHAND:
        if(this->mRightHand.isEmpty())
        {
            if(this->mLeftHand.isEmpty())
                return true;

            else if(this->mLeftHand.mItem.itemType != item.mItem.itemType)
                return true;
        }

        else if(item.mItem.equipLoc == Level::Item::eqTWOHAND)
        {
            if(item.mItem.equipLoc == Level::Item::eqTWOHAND && this->mLeftHand.isEmpty())
                return true;

        }
        break;
    case Level::Item::eqBODY:
        if(item.mItem.equipLoc == Level::Item::eqBODY && this->mBody.isEmpty())
            return true;
        break;

    case Level::Item::eqHEAD:
        if(item.mItem.equipLoc == Level::Item::eqHEAD && this->mHead.isEmpty())
            return true;
        break;

    case Level::Item::eqLEFTRING:
        if(item.mItem.equipLoc == Level::Item::eqRING)
        {
            if(this->mLeftRing.isEmpty()) return true;
        }
        break;
    case Level::Item::eqRIGHTRING:

        if(item.mItem.equipLoc == Level::Item::eqRING)
        {
            if(this->mRightRing.isEmpty()) return true;
        }
        break;

    case Level::Item::eqAMULET:
        if(item.mItem.equipLoc == Level::Item::eqAMULET && this->mAmulet.isEmpty())
            return true;
        break;
        /*
             * When putting an item in the inventory we must check if it will fit!
             * */

    case Level::Item::eqINV:
        if(x <= 9 && y <=3)
        {
            if(mInventoryBox[y][x].isEmpty())
            {
                for(uint8_t i=y;i < item.mSizeY; ++i)
                {
                    for(uint8_t j=x; j < item.mSizeX;++j)
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
    case Level::Item::eqBELT:
        if(item.mItem.equipLoc == Level::Item::eqUNEQUIP && item.mItem.itemType == Level::Item::itPOT)
        {
            if(beltX <= 7)
            {
                if(mBelt[beltX].isEmpty()) return true;
            }

        }
        break;
    case Level::Item::eqFLOOR:
        return true;
    case Level::Item::eqCURSOR:
        return !this->mCursorHeld.isEmpty();
    default:
        return false;


    }

    return false;
}
//TODO: When stats have implemented add checks for requirements to wear/wield items
void Inventory::putItem(Level::Item item,
                        Level::Item::equipLoc equipType,
                        Level::Item::equipLoc from,
                        uint8_t y,
                        uint8_t x,
                        uint8_t beltX)
{
    if(canPlaceItem(item, equipType, y, x, beltX))
    {
        switch(equipType)
        {
        case Level::Item::eqLEFTHAND:
            if(item.mItem.equipLoc == Level::Item::eqONEHAND)
            {
                this->mLeftHand=item;
                removeItem(item, from, beltX);
            }
            else if(item.mItem.equipLoc == Level::Item::eqTWOHAND)
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
                        putItem(this->mRightHand, Level::Item::eqINV, Level::Item::eqRIGHTHAND, auto_fit_y, auto_fit_x);
                        this->mRightHand = item;
                        return;
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
                }

            }
            break;
        case Level::Item::eqRIGHTHAND:
            this->mRightHand=item;
            removeItem(item, from, beltX);
            break;
            /*
                 * When wielding a two-handed weapon the game will unload the right
                 * hand weapon if there is one. If there is space in your inventory
                 * it will automatically put it there. If there is not it will give
                 * it to you to sort out for yourself.
                 * */

        case Level::Item::eqBODY:
            mBody = item;
            removeItem(item, from, beltX);
            break;

        case Level::Item::eqHEAD:
            mHead = item;
            removeItem(item, from, beltX);
            break;

        case Level::Item::eqLEFTRING:
            mLeftRing = item;
            removeItem(item, from, beltX);
            break;
        case Level::Item::eqRIGHTRING:

            mRightRing = item;
            removeItem(item, from, beltX);
            break;

        case Level::Item::eqAMULET:
            mAmulet = item;
            break;

            /*
                 * For every space the item takes up in the inventory we leave
                 * a reference to the item in the corresponding entry in mInventoryBox.
                */
        case Level::Item::eqINV:
            if(item.mMaxCount > 1)
            {

                for(uint8_t i=0;i<4;i++)
                {
                    for(uint8_t j=0;j<10;j++)
                    {

                        if((mInventoryBox[i][j].mBaseId == item.mBaseId) && (mInventoryBox[i][j].mCount+item.mCount) <=item.mMaxCount && (mInventoryBox[i][j].mUniqueId != item.mUniqueId))
                        {
                            mInventoryBox[i][j].mCount+=item.mCount;
                            removeItem(item, from, item.mInvX, item.mInvY, item.mBeltX);
                            return;
                        }
                        else if(mInventoryBox[i][j] == item)
                        {
                            item.mInvX = x;
                            item.mInvY = y;
                            mInventoryBox[y][x] = item;

                            mInventoryBox[i][j] = Level::Item();



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



                }
            }

            removeItem(item, from, beltX, y, x);
            break;
        case Level::Item::eqBELT:
            item.mBeltX = beltX;
            removeItem(item, from, beltX);
            this->mBelt[beltX]=item;
            break;
        case Level::Item::eqFLOOR:
            break;

        default:
            return;

        }
    }
}
Level::Item Inventory::getItemAt(Level::Item::equipLoc type, uint8_t y, uint8_t x, uint8_t beltX)
{

    Level::Item item;

    switch(type)
    {
        case Level::Item::eqLEFTHAND: item=mLeftHand; break;
        case Level::Item::eqLEFTRING: item=mLeftRing; break;
        case Level::Item::eqRIGHTHAND: item=mRightHand; break;
        case Level::Item::eqRIGHTRING: item=mRightRing; break;
        case Level::Item::eqBODY: item=mBody; break;
        case Level::Item::eqHEAD: item=mHead; break;
        case Level::Item::eqAMULET: item=mAmulet; break;
        case Level::Item::eqINV: item=mInventoryBox[y][x]; break;
        case Level::Item::eqBELT: item=mBelt[beltX]; break;
        default: break;

    }


    return item;
}

void Inventory::removeItem(
        Level::Item item,
       Level::Item::equipLoc from,
        uint8_t beltX,
        uint8_t invY,
        uint8_t invX)
{
    switch(from)
    {
    case Level::Item::eqLEFTHAND:
        this->mLeftHand = Level::Item();
        break;

    case Level::Item::eqRIGHTHAND:
        this->mRightHand=Level::Item();
        break;

    case Level::Item::eqLEFTRING:
        this->mLeftRing=Level::Item();
        break;
    case Level::Item::eqRIGHTRING:
        this->mRightRing =Level::Item();
        break;

    case Level::Item::eqBELT:
        mBelt[beltX] =Level::Item();
        break;

    case Level::Item::eqCURSOR:
        mCursorHeld = Level::Item();
        break;

    case Level::Item::eqAMULET:
        mAmulet = Level::Item();
        break;

    case Level::Item::eqHEAD:
        mHead = Level::Item();
        break;

    case Level::Item::eqBODY:
        mBody = Level::Item();
        break;

    case Level::Item::eqINV:
        for(uint8_t i=invY;i < invY+item.mSizeY-1;i++)
        {
            for(uint8_t j=invX;j < invX+item.mSizeX;j++)
            {
                mInventoryBox[i][j] = Level::Item();

            }
        }
        break;

    case Level::Item::eqONEHAND:
    case Level::Item::eqTWOHAND:
    case Level::Item::eqUNEQUIP:
    case Level::Item::eqFLOOR:
    default:
        return;





    }

}
bool Inventory::fitsAt(Level::Item item, uint8_t y, uint8_t x)
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
            ss << "| "<< mInventoryBox[i][j].mItem.itemName;
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
    std::cout << "head: " << mHead.mItem.itemName << std::endl;
    std::cout << "mBody: " << mBody.mItem.itemName << std::endl;
    std::cout << "mAmulet: " << mAmulet.mItem.itemName << std::endl;
    std::cout << "mRightHand: " << mRightHand.mItem.itemName << std::endl;
    std::cout << "mLeftHand: " << mLeftHand.mItem.itemName << std::endl;
    std::cout << "mLeftRing: " << mLeftRing.mItem.itemName << std::endl;
    std::cout << "mRightRing: " << mRightRing.mItem.itemName << std::endl;
    std::cout << "mCursorHeld: " << mCursorHeld.mItem.itemName << std::endl;
    std::stringstream printbelt;
    printbelt <<  "mBelt: ";
    for(size_t i=0; i<8; i++)
    {
        printbelt << mBelt[i].mItem.itemName << ", ";
    }
    std::cout << printbelt.str() << std::endl;
}


}
