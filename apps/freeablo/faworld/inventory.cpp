#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include <diabloexe/diabloexe.h>
#include "inventory.h"

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
        Item * gold = new Item();
        DiabloExe::DiabloExe exe = DiabloExe::DiabloExe();
        gold->mEmpty=false;
        gold->mItem = exe.getItem("Gold");
        gold->mSizeX=1;
        gold->mSizeY=1;
        inv->putItem(gold, eqINV, eqFLOOR, 0, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 1, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 2, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 3, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 4, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 5, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 6, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 7, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 8, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 9, 0);
        inv->putItem(gold, eqINV, eqFLOOR, 0, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 1, 1);
        //inv->putItem(gold, eqINV, eqFLOOR, 2, 1);
        //inv->putItem(gold, eqINV, eqFLOOR, 3, 1);
        //inv->putItem(gold, eqINV, eqFLOOR, 4, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 5, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 6, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 7, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 8, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 9, 1);
        inv->putItem(gold, eqINV, eqFLOOR, 0, 2);
        inv->putItem(gold, eqINV, eqFLOOR, 1, 2);
        //inv->putItem(gold, eqINV, eqFLOOR, 2, 2);
        //inv->putItem(gold, eqINV, eqFLOOR, 3, 2);
        //inv->putItem(gold, eqINV, eqFLOOR, 4, 2);
        inv->putItem(gold, eqINV, eqFLOOR, 5, 2);
        inv->putItem(gold, eqINV, eqFLOOR, 6, 2);
        inv->putItem(gold, eqINV, eqFLOOR, 7, 2);
        //inv->putItem(gold, eqINV, eqFLOOR, 8, 2);
        //inv->putItem(gold, eqINV, eqFLOOR, 9, 2);
        inv->putItem(gold, eqINV, eqFLOOR, 0, 3);
        inv->putItem(gold, eqINV, eqFLOOR, 1, 3);
        //inv->putItem(gold, eqINV, eqFLOOR, 2, 3);
        //inv->putItem(gold, eqINV, eqFLOOR, 3, 3);
        inv->putItem(gold, eqINV, eqFLOOR, 4, 3);
        inv->putItem(gold, eqINV, eqFLOOR, 5, 3);
        inv->putItem(gold, eqINV, eqFLOOR, 6, 3);
        inv->putItem(gold, eqINV, eqFLOOR, 7, 3);
        //inv->putItem(gold, eqINV, eqFLOOR, 8, 3);
        //inv->putItem(gold, eqINV, eqFLOOR, 9, 3);
        Item * bow = new Item();
        Item * buckler = new Item();
        buckler->mEmpty=false;
        buckler->mItem = exe.getItem("Buckler");
        buckler->mSizeX=3;
        buckler->mSizeY=3;

        bow->mEmpty=false;
        bow->mItem = exe.getItem("Composite Bow");
        bow->mSizeX=2;
        bow->mSizeY=3;
        inv->putItem(buckler, eqONEHAND, eqFLOOR, 0, 0, 0, eqRIGHTHAND);
        inv->putItem(bow, eqTWOHAND, eqFLOOR, 1, 1);
        //inv->putItem(bow, eqFLOOR, eqTWOHAND, 1, 1);


        inv->dump();
        return inv;

    }

    bool Inventory::canPlaceItem(
            Item * item,
            equipLoc equipType,
            uint8_t x,
            uint8_t y,
            uint8_t beltX,
            equipLoc hand ,
            equipLoc ringSlot)
    {
        switch(equipType)
        {
            /* Characters in Diablo are ambidextrous so we can put a shield in the left
             * or right hand and a sword in the left or right hand. You cannot duel
             * wield swords nor can you carry two shields. Shields are classified as
             * armour in the game.
             *
             * */
            case eqONEHAND:
                if(item->mItem.equipLoc == eqONEHAND)
                {
                    if(!hand) //hand = 0 => left hand
                    {
                        if(this->mLeftHand->isEmpty())
                        {
                            if(this->mRightHand->isEmpty())
                                return true;

                            else if(this->mRightHand->mItem.itemType != item->mItem.itemType)
                                return true;
                        }
                    }
                    if(hand)
                    {
                        if(this->mRightHand->isEmpty())
                        {
                            if(this->mLeftHand->isEmpty())
                                return true;

                            else if(this->mLeftHand->mItem.itemType != item->mItem.itemType)
                                return true;
                        }
                    }
                }
                break;
            case eqTWOHAND:
                if(item->mItem.equipLoc == eqTWOHAND && this->mLeftHand->isEmpty())
                  return true;
                break;

            case eqBODY:
                if(item->mItem.equipLoc == eqBODY && this->mBody->isEmpty())
                  return true;
                break;

            case eqHEAD:
                if(item->mItem.equipLoc == eqHEAD && this->mHead->isEmpty())
                  return true;
                break;

            case eqRING:
                if(item->mItem.equipLoc == eqRING)
                {
                    if(ringSlot==eqLEFTHAND)
                    {
                        if(this->mLeftRing->isEmpty()) return true;


                    }
                    else
                    {
                        if(this->mRightRing->isEmpty()) return true;

                    }
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
                                    //std::cout <<"slot occupied " << (size_t)x << "," << (size_t)y << std::endl;
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
            case eqCURSOR:
            case eqRIGHTHAND:
            case eqLEFTHAND:
            case eqRIGHTRING:
            case eqLEFTRING:
            default:
            return false;


        }

        return false;
    }
    //TODO: When stats have implemented add checks for requirements to wear/wield items
    void Inventory::putItem(Item* item,
                            equipLoc equipType,
                            equipLoc from,
                            uint8_t x,
                            uint8_t y,
                            uint8_t beltX,
                            equipLoc hand,
                            equipLoc ringSlot)
    {
      if(canPlaceItem(item, equipType, x, y, beltX, hand, ringSlot))
        {
            switch(equipType)
            {
                case eqONEHAND:
                    if(hand==eqLEFTHAND)
                    {
                        this->mLeftHand=item;
                        removeItem(item, from);
                    }
                    else
                    {
                        this->mRightHand=item;
                        removeItem(item, from);
                    }
                    break;
                /*
                 * When wielding a two-handed weapon the game will unload the right
                 * hand weapon if there is one. If there is space in your inventory
                 * it will automatically put it there. If there is not it will give
                 * it to you to sort out for yourself.
                 * */
                case eqTWOHAND:

                    if(!this->mRightHand->isEmpty())
                    {
                        this->mLeftHand = item;
                        //std::cout <<"remove0\n";
                        bool foundItem=false;
                        bool foundSpace=false;
                        uint8_t auto_fit_x=255, auto_fit_y=255;
                        for(uint8_t i=0;i<4/*-(this->mRightHand->mSizeX)*/;i++)
                        {

                            for(uint8_t j=0;j<10/*-(this->mRightHand->mSizeY)*/;j++)
                            {
                                foundItem=false;

                                if(foundSpace)
                                    break;
                                if(mInventoryBox[i][j]->isEmpty())
                                {
                                    if(i+this->mRightHand->mSizeY < 5 && j+this->mRightHand->mSizeX < 11)
                                    {
                                        for(uint8_t k=i;k<i+this->mRightHand->mSizeY;k++)
                                        {
                                            for(uint8_t l=j;l<j+this->mRightHand->mSizeX;l++)
                                            {

                                                std::cout <<"[4th Loop] State:[i][j][k][l]::["<<(size_t)i
                                                         <<"]["
                                                        <<(size_t)j
                                                       <<"]["
                                                      <<(size_t)k
                                                     <<"]["
                                                    <<(size_t)l
                                                   <<"]"
                                                  <<std::endl;

                                                if(foundItem)
                                                    break;
                                                if(!mInventoryBox[k][l]->isEmpty())
                                                {
                                                    foundItem = true;

                                                }
                                                if(l==(j + this->mRightHand->mSizeX-1) &&
                                                        k==(i + this->mRightHand->mSizeY-1) &&
                                                        !foundItem)
                                                {
                                                    auto_fit_x = j;
                                                    auto_fit_y = i;
                                                    foundSpace = true;

                                                }

                                            }
                                        }
                                   }

                                }
                                else
                                {
                                    std::cout<<"[2nd Loop] Found item at: [i][j]::["<<(size_t)i <<"][" << (size_t)j<<"]"<< ", skipping." << std::endl;
                                    continue;
                                }

                            }
                        }
                        if(auto_fit_x !=255)
                        {
                            std::cout <<"found space\n";
                            putItem(this->mRightHand, eqINV, eqRIGHTHAND, auto_fit_x, auto_fit_y);
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
                    break;

                case eqBODY:
                    this->mBody = item;
                    //std::cout <<"remove1\n";
                    removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
                    break;

                case eqHEAD:
                    this->mHead = item;
                    //std::cout <<"remove2\n";
                    removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
                    break;

                case eqRING:
                    if(ringSlot==eqLEFTRING)
                    {
                        if(this->mLeftRing->isEmpty())
                        {
                            this->mLeftRing = item;
                            //std::cout <<"remove3\n";
                            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
                        }
                    }
                    else
                    {
                        if(this->mRightRing->isEmpty())
                        {
                            this->mRightRing = item;
                            //std::cout <<"remove4\n";
                            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);

                        }
                    }
                    break;

                case eqAMULET:
                    if(this->mAmulet->isEmpty())
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
//                            std::cout <<"[i][j]::"<<"["<<(size_t)i<<"]["<<(size_t)j<<"]"<<std::endl;
                            //std::cout <<"remove5\n";
                            removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
//                            std::cout << "item: " << (size_t)item<< std::endl;
                            item->mInvX=x;
                            item->mInvY=y;
                            mInventoryBox[y+i][x+j] = item;
//                            std::cout << "made it to " << __LINE__ << std::endl;


                        }
                    }
                    break;
                case eqBELT:
                    //std::cout <<"remove6\n";
                    removeItem(item, from, item->mInvX, item->mInvY, item->mBeltX);
                    item->mBeltX=beltX;
                    this->mBelt[beltX]=item;
                    break;
                default:
                return;

            }
          }
        else
        {
        }
//       std::cout << "Reached end of putItem.. returning" << std::endl;
    }
    void Inventory::removeItem(
                               Item* item,
                               equipLoc from,
                               uint8_t x,
                               uint8_t y,
                               uint8_t beltX)
    {
        //std::cout << "removeItem called\n";
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
               // std::cout << "correct case: "<< (size_t)x <<", " << (size_t)y <<"\n";

                for(uint8_t i=y;i<=y+(item->mSizeY-1);i++)
                {
                    for(uint8_t j=x;j<=x+(item->mSizeX-1);j++)
                    {
                        mInventoryBox[i][j] = new Item();
                        //std::cout <<"removing item\n";

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
            //std::cout<<"------------------------------------------" << std::endl;
            for(uint8_t j=0;j<10;j++)
            {
//                std::cout << "[i][j]::["<<(size_t)i << "]["<<(size_t)j <<"]" << std::endl;
                ss << "| "<< mInventoryBox[i][j]->mItem.itemName<< ",\t";

            }
            ss << " |" << std::endl;




        }
//        std::cout << "made it here: " << __LINE__ << std::endl;
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
    }


}
