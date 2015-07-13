#ifndef INVENTORY_H
#define INVENTORY_H
#include <stdint.h>
#include "item.h"

namespace FAWorld
{
    class Actor;
    class Inventory
    {


    private:
        Item inventoryBox[4][10];
        Item belt[8];
        Item head;
        Item body;
        Item leftRing;
        Item rightRing;
        Item amulet;
        Item leftHand;
        Item rightHand;
        Inventory();
        bool canPlaceItem(Item item, uint8_t equipType,
                          uint8_t x=0,
                          uint8_t y=0,
                          uint8_t barX=0,
                          uint8_t hand=0,
                          uint8_t ringSlot=0);


    public:
        Inventory testInv();
        void putItem(Item item, uint8_t equipType,
                     uint8_t x=0,
                     uint8_t y=0,
                     uint8_t barX=0,
                     uint8_t hand=0,
                     uint8_t ringSlot=0);

        typedef enum{eqONEHAND=1,
                    eqTWOHAND=2,
                    eqBODY=3,
                    eqHEAD=4,
                    eqRING=5,
                    eqAMULET=6,
                    eqUNEQUIP=7,
                    eqTOINV=8,
                    eqTOBELT=9}equipLoc;

        typedef enum{itWEAPON=1,
                     itARMOUR=2,
                     itPOT=3,
                     itGOLD=4,
                     itNOVELTY=5}itemType;

    friend class Actor;



    };


}

#endif // INVENTORY_H
