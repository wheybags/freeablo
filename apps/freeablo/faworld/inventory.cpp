#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include "inventory.h"
#include "itemmanager.h"
namespace FAWorld
{
Inventory::Inventory()
{
    mInventoryBox = new Item**[4];

    for(uint8_t i=0; i<4;i++)
    {
        mInventoryBox[i] = new Item*[10];
        for(uint8_t j=0;j<10;j++)
        {
            mInventoryBox[i][j] = new Item();
        }

    }

    mBelt = new Item*[8];
    for(uint8_t i=0;i<8;i++)
    {
        mBelt[i] = new Item();
    }
    mHead = new Item();
    mBody = new Item();
    mAmulet = new Item();
    mLeftHand = new Item();
    mRightHand = new Item();
    mLeftRing = new Item();
    mRightRing = new Item();
    mCursorHeld = new Item();


}
Inventory::~Inventory()
{
    delete[] mInventoryBox;
    delete[] mBelt;
    delete mHead;
    delete mBody;
    delete mAmulet;
    delete mLeftHand;
    delete mRightHand;
    delete mLeftRing;
    delete mRightRing;
}

Inventory *Inventory::testInv()
{
    Inventory * inv = new Inventory();
    ItemManager * itemManager = ItemManager::getItemManager();
    Item * gold = itemManager->getBaseItem(38);
    Item * bow  = itemManager->getBaseItem(23);
    Item * buckler = itemManager->getBaseItem(15);
    std::cout << "gold: (" <<(size_t) gold->mSizeX <<", " << (size_t)gold->mSizeY <<") "<< (size_t)gold->mItem.itemType <<std::endl;
    inv->putItem(gold, eqINV, eqFLOOR, 0, 0);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 1);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 2);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 3);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 4);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 5);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 6);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 7);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 8);
    inv->putItem(gold, eqINV, eqFLOOR, 0, 9);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 0);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 1);
    //inv->putItem(gold, eqINV, eqFLOOR, 1, 2);
    //inv->putItem(gold, eqINV, eqFLOOR, 1, 3);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 4);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 5);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 6);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 7);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 8);
    inv->putItem(gold, eqINV, eqFLOOR, 1, 9);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 0);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 1);
    //inv->putItem(gold, eqINV, eqFLOOR, 2, 2);
    //inv->putItem(gold, eqINV, eqFLOOR, 2, 3);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 4);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 5);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 6);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 7);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 8);
    inv->putItem(gold, eqINV, eqFLOOR, 2, 9);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 0);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 1);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 2);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 3);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 4);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 5);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 6);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 7);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 8);
    inv->putItem(gold, eqINV, eqFLOOR, 3, 9);
    inv->putItem(buckler, eqRIGHTHAND, eqFLOOR, 0, 0, 0);
    inv->putItem(bow, eqLEFTHAND, eqFLOOR, 1, 1);
    inv->putItem(bow, eqFLOOR, eqTWOHAND, 1, 1);


    inv->dump();
    return inv;

}

bool Inventory::canPlaceItem(
        Item * item,
        equipLoc equipType,
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
    case eqLEFTHAND:
        if(item->mItem.equipLoc == eqONEHAND)
        {
            if(this->mLeftHand->isEmpty())
            {
                if(this->mRightHand->isEmpty())
                    return true;

                else if(this->mRightHand->mItem.itemType != item->mItem.itemType)
                    return true;
            }
        }
        else if(item->mItem.equipLoc == eqTWOHAND)
        {
            if(item->mItem.equipLoc == eqTWOHAND && this->mLeftHand->isEmpty())
                return true;

        }
        break;
    case eqRIGHTHAND:
        if(this->mRightHand->isEmpty())
        {
            if(this->mLeftHand->isEmpty())
                return true;

            else if(this->mLeftHand->mItem.itemType != item->mItem.itemType)
                return true;
        }

        else if(item->mItem.equipLoc == eqTWOHAND)
        {
            if(item->mItem.equipLoc == eqTWOHAND && this->mLeftHand->isEmpty())
                return true;

        }
        break;
    case eqBODY:
        if(item->mItem.equipLoc == eqBODY && this->mBody->isEmpty())
            return true;
        break;

    case eqHEAD:
        if(item->mItem.equipLoc == eqHEAD && this->mHead->isEmpty())
            return true;
        break;

    case eqLEFTRING:
        if(item->mItem.equipLoc == eqRING)
        {
            if(this->mLeftRing->isEmpty()) return true;
        }
        break;
    case eqRIGHTRING:

        if(item->mItem.equipLoc == eqRING)
        {
            if(this->mRightRing->isEmpty()) return true;
        }
        break;

    case eqAMULET:
        if(item->mItem.equipLoc == eqAMULET && this->mAmulet->isEmpty())
            return true;
        break;
        /*
             * When putting an item in the inventory we must check if it will fit!
             * */

    case eqINV:
        if(x <= 9 && y <=3)
        {
            if(mInventoryBox[y][x]->isEmpty())
            {
                for(uint8_t i=y;i < item->mSizeY; ++i)
                {
                    for(uint8_t j=x; j < item->mSizeX;++j)
                    {

                        if(!mInventoryBox[i][j]->isEmpty())
                        {
                            return false;
                        }
                    }

                }
                return true;
            }

        }
        break;
    case eqBELT:
        if(item->mItem.equipLoc == eqUNEQUIP && item->mItem.itemType == itPOT)
        {
            if(beltX <= 7)
            {
                if(mBelt[beltX]->isEmpty()) return true;
            }

        }
        break;
    case eqFLOOR:
        return true;
    case eqCURSOR:
        return !this->mCursorHeld->isEmpty();
    default:
        return false;


    }

    return false;
}
//TODO: When stats have implemented add checks for requirements to wear/wield items
void Inventory::putItem(Item* item,
                        equipLoc equipType,
                        equipLoc from,
                        uint8_t y,
                        uint8_t x,
                        uint8_t beltX,
                        boost::tuple<size_t,size_t,size_t> * floorPosition)
{
    ItemManager * itemManager = ItemManager::getItemManager();
    if(canPlaceItem(item, equipType, y, x, beltX))
    {
        switch(equipType)
        {
        case eqLEFTHAND:
            if(item->mItem.equipLoc == eqONEHAND)
            {
                this->mLeftHand=item;
                removeItem(item, from);
            }
            else if(item->mItem.equipLoc == eqTWOHAND)
            {

                if(!this->mRightHand->isEmpty())
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
                            if(mInventoryBox[i][j]->isEmpty())
                            {
                                if(i+this->mRightHand->mSizeY < 5 && j+this->mRightHand->mSizeX < 11)
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
                        putItem(this->mRightHand, eqINV, eqRIGHTHAND, auto_fit_y, auto_fit_x);
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
        case eqRIGHTHAND:
            this->mRightHand=item;
            removeItem(item, from);
            break;
            /*
                 * When wielding a two-handed weapon the game will unload the right
                 * hand weapon if there is one. If there is space in your inventory
                 * it will automatically put it there. If there is not it will give
                 * it to you to sort out for yourself.
                 * */

        case eqBODY:
            this->mBody = item;
            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
            break;

        case eqHEAD:
            this->mHead = item;
            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
            break;

        case eqLEFTRING:
            this->mLeftRing = item;
            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
            break;
        case eqRIGHTRING:
            this->mRightRing = item;
            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
            break;

        case eqAMULET:
            this->mAmulet = item;
            break;

            /*
                 * For every space the item takes up in the inventory we leave
                 * a reference to the item in the corresponding entry in mInventoryBox.
                */
        case eqINV:
            for(uint8_t i=0;i<item->mSizeY;i++)
            {
                for(uint8_t j=0;j<item->mSizeX;j++)
                {
                    removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
                    item->mInvX=x;
                    item->mInvY=y;
                    mInventoryBox[y+i][x+j] = item;


                }
            }
            break;
        case eqBELT:
            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
            item->mBeltX=beltX;
            this->mBelt[beltX]=item;
            break;
        case eqFLOOR:

            break;

        default:
            return;

        }
        auto tempInvPos = new std::pair<uint8_t, uint8_t>(x,y);
        itemManager->putItem(item, equipType, from, tempInvPos, beltX, floorPosition);
    }
}
void Inventory::removeItem(
        Item* item,
        equipLoc from,
        uint8_t x,
        uint8_t y,
        uint8_t beltX)
{
    switch(from)
    {
    case eqLEFTHAND:
        this->mLeftHand=new Item();
        break;

    case eqRIGHTHAND:
        this->mRightHand=new Item();
        break;

    case eqLEFTRING:
        this->mLeftRing=new Item();
        break;
    case eqRIGHTRING:
        this->mRightRing=new Item();
        break;

    case eqBELT:
        mBelt[beltX] = new Item();
        break;

    case eqCURSOR:
        mCursorHeld = new Item();
        break;

    case eqAMULET:
        mAmulet = new Item();
        break;

    case eqHEAD:
        mHead = new Item();
        break;

    case eqBODY:
        mBody = new Item();
        break;

    case eqINV:

        for(uint8_t i=y;i<=y+(item->mSizeY-1);i++)
        {
            for(uint8_t j=x;j<=x+(item->mSizeX-1);j++)
            {
                mInventoryBox[i][j] = new Item();

            }
        }
        break;

    case eqONEHAND:
    case eqTWOHAND:
    case eqUNEQUIP:
    case eqFLOOR:
    default:
        return;





    }

}
bool Inventory::fitsAt(Item *item, uint8_t y, uint8_t x)
{
    bool foundItem=false;

    if(y+item->mSizeY < 5 && x+item->mSizeX < 11)
    {
        for(uint8_t k=y;k<y+item->mSizeY;k++)
        {
            for(uint8_t l=x;l<x+item->mSizeX;l++)
            {
                if(foundItem)
                    break;
                if(!mInventoryBox[k][l]->isEmpty())
                {
                    foundItem = true;
                }
                if(l==(x + item->mSizeX-1) &&
                        k==(y + item->mSizeY-1) &&
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
            ss << "| "<< mInventoryBox[i][j]->mItem.itemName<< ",\t";

        }
        ss << " |" << std::endl;




    }
    size_t len = ss.str().length()/2;
    std::string tops = "";
    tops.append(len,'-');
    std::cout << tops << std::endl << ss.str() << tops << std::endl;
    std::cout << "head: " << mHead->mItem.itemName << std::endl;
    std::cout << "mBody: " << mBody->mItem.itemName << std::endl;
    std::cout << "mAmulet: " << mAmulet->mItem.itemName << std::endl;
    std::cout << "mRightHand: " << mRightHand->mItem.itemName << std::endl;
    std::cout << "mLeftHand: " << mLeftHand->mItem.itemName << std::endl;
    std::cout << "mLeftRing: " << mLeftRing->mItem.itemName << std::endl;
    std::cout << "mRightRing: " << mRightRing->mItem.itemName << std::endl;
    std::cout << "mCursorHeld: " << mCursorHeld->mItem.itemName << std::endl;
    std::stringstream printbelt;
    printbelt <<  "mBelt: ";
    for(size_t i=0; i<8; i++)
    {
        printbelt << mBelt[i]->mItem.itemName << ", ";
    }
    std::cout << printbelt.str() << std::endl;
}


}
