#include <stdint.h>
#include <string>
#include <iostream>
#include <diabloexe/diabloexe.h>
#include "inventory.h"

namespace FAWorld
{
    Inventory::Inventory()
    {
        head=emptyItem;
        body=emptyItem;
        leftRing=emptyItem;
        rightRing=emptyItem;
        leftHand=emptyItem;
        rightHand=emptyItem;
        amulet=emptyItem;
        for(uint8_t i=0;i<3;i++)
        {
            for(uint8_t j=0;j<9;j++)
            {
                inventoryBox[i][j] = emptyItem;
            }

        }

        for(uint8_t i=0;i<7;i++)
        {
            belt[i] = emptyItem;
        }



    }

    Inventory Inventory::testInv()
    {
        Inventory inv = Inventory();
        Item gold = Item();
        DiabloExe::DiabloExe exe = DiabloExe::DiabloExe();
        gold.item = exe.getItem("Gold");
        gold.sizeX=1;
        gold.sizeY=1;
        std::cout << "name: " << gold.item.itemName << std::endl;
        inv.putItem(gold, eqTOINV, 0, 0);
        return inv;

    }

    bool Inventory::canPlaceItem(Item item, uint8_t equipType, uint8_t x, uint8_t y, uint8_t barX, uint8_t hand , uint8_t ringSlot)
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
                if(item.item.equipLoc == eqONEHAND)
                {
                    if(!hand) //hand = 0 => left hand
                    {
                        if(&this->leftHand == &emptyItem)
                        {
                            if(&this->rightHand == &emptyItem)
                                return true;

                            else if(this->rightHand.item.itemType != item.item.itemType)
                                return true;
                        }
                    }
                    if(hand)
                    {
                        if(&this->rightHand == &emptyItem)
                        {
                            if(&this->leftHand == &emptyItem)
                                return true;

                            else if(this->leftHand.item.itemType != item.item.itemType)
                                return true;
                        }
                    }
                }
            case eqTWOHAND:
                if(item.item.equipLoc == eqTWOHAND && &this->leftHand==&emptyItem) return true;
            case eqBODY:
                if(item.item.equipLoc == eqBODY && &this->body==&emptyItem) return true;
            case eqHEAD:
                if(item.item.equipLoc == eqHEAD && &this->head==&emptyItem) return true;
            case eqRING:
                if(item.item.equipLoc == eqRING)
                {
                    if(!ringSlot) //ringSlot=0 => left hand ring
                    {
                        if(&this->leftRing == &emptyItem) return true;


                    }
                    else //ringSlot=1 => right hand ring
                    {
                        if(&this->rightRing == &emptyItem) return true;

                    }
                }
            case eqAMULET:
                if(item.item.equipLoc == eqAMULET && &this->amulet==&emptyItem) return true;
            /*
             * When putting an item in the inventory we must check if it will fit!
             * */

            case eqTOINV:
                if(x <= 9 && y <=3)
                {
                    if(&inventoryBox[y][x] == &emptyItem)
                    {
                        for(uint8_t i=0;i < item.sizeY; i++)
                        {
                            for(uint8_t j=0; j < item.sizeX;j++)
                            {
                                if(&inventoryBox[i][j] != &emptyItem) return false;
                            }

                        }
                        return true;
                    }

                }
            case eqTOBELT:
                if(item.item.equipLoc == eqUNEQUIP && item.item.itemType == itPOT)
                {
                    if(barX <= 7)
                    {
                        if(&belt[barX] == &emptyItem) return true;
                    }

                }
        }

        return false;
    }
    //TODO: When stats have implemented add checks for requirements to wear/wield items
    void Inventory::putItem(Item item, uint8_t equipType, uint8_t x, uint8_t y, uint8_t barX, uint8_t hand, uint8_t ringSlot)
    {
        if(canPlaceItem(item, equipType, x, y, barX, hand, ringSlot))
        {
            switch(equipType)
            {
                case eqONEHAND:
                    if(!hand) this->leftHand=item;
                    if(hand)  this->rightHand=item;
                /*
                 * When wielding a two-handed weapon the game will unload the right
                 * hand weapon if there is one. If there is space in your inventory
                 * it will automatically put it there. If there is not it will give
                 * it to you to sort out for yourself.
                 * */
                case eqTWOHAND:
                    if(&this->rightHand != &emptyItem)
                    {
                        this->leftHand = item;

                        for(uint8_t i=0;i<=9;i++)
                        {
                            for(uint8_t j=0;j<=3;j++)
                            {
                                if(&inventoryBox[i][j] == &emptyItem)
                                {
                                    bool brokeLoop=false;
                                    for(uint8_t k=0;k<this->rightHand.sizeY;k++)
                                    {
                                        for(uint8_t l=0;l<this->rightHand.sizeX;l++)
                                        {
                                            brokeLoop=false;
                                            if(&inventoryBox[k][l] != &emptyItem)
                                            {
                                                brokeLoop = true;
                                                break;

                                            }
                                            if(!brokeLoop && k==this->rightHand.sizeY-1 && l==this->rightHand.sizeX-1)
                                            {
                                                putItem(this->rightHand, eqTOINV, i, j);
                                                return;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        /* TODO: Add case to give the item to the cursor so the player can do
                                 inventory management. Can't do because because it requires a
                                 python interface that doesn't exist yet. */


                    }
                    else
                    {
                        this->leftHand = item;
                        this->rightHand = item;
                    }
                case eqBODY:
                    this->body = item;
                case eqHEAD:
                    this->head = item;
                case eqRING:
                    if(!ringSlot)
                    {
                        if(&this->leftRing == &emptyItem)
                            this->leftRing = item;
                    }
                    else
                    {
                        if(&this->rightRing == &emptyItem)
                            this->rightRing = item;
                    }
                case eqAMULET:
                    if(&this->amulet == &emptyItem) this->amulet = item;

                /*
                 * For every space the item takes up in the inventory we leave
                 * a reference to the item in the corresponding entry in inventoryBox.
                */
                case eqTOINV:
                    item.invX=x;
                    item.invY=y;
                    for(uint8_t i=0;i<item.sizeY;i++)
                    {
                        for(uint8_t j=0;j<item.sizeX;j++)
                        {
                            inventoryBox[i][j] = item;

                        }
                    }
                case eqTOBELT:
                    this->belt[barX]=item;

            }
        }
    }
}
