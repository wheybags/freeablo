#ifndef INVENTORY_H
#define INVENTORY_H
#include <stdint.h>
#include "item.h"

namespace FAWorld
{
    class Actor;
    class Inventory
    {
    public:
        typedef enum{eqONEHAND=1,
                    eqTWOHAND=2,
                    eqBODY=3,
                    eqHEAD=4,
                    eqRING=5,
                    eqAMULET=6,
                    eqUNEQUIP=7,
                    eqINV=8,
                    eqBELT=9,
                    eqFLOOR=10,
                    eqCURSOR=11,
                    eqRIGHTHAND=12,
                    eqLEFTHAND=13,
                    eqRIGHTRING=14,
                    eqLEFTRING=15} equipLoc;

        typedef enum{itWEAPON=1,
                     itARMOUR=2,
                     itPOT=3,
                     itGOLD=4,
                     itNOVELTY=5}itemType;

    private:
        Item*** mInventoryBox;
        Item** mBelt;
        Item* mHead;
        Item* mBody;
        Item* mLeftRing;
        Item* mRightRing;
        Item* mAmulet;
        Item* mLeftHand;
        Item* mRightHand;
        Item* mCursorHeld;
        Inventory();
        ~Inventory();
        bool canPlaceItem(Item *item,
                equipLoc equipType,
                uint8_t x=0,
                uint8_t y=0,
                uint8_t beltX=0,
                equipLoc hand=eqLEFTHAND,
                equipLoc ringSlot=eqLEFTRING);
        bool fitsAt(Item * item, uint8_t y, uint8_t x);






    public:
        Inventory * testInv();
        void putItem(Item * item,
                     equipLoc equipType,
                     equipLoc from,
                     uint8_t y=0,
                     uint8_t x=0,
                     uint8_t beltX=0,
                     equipLoc hand=eqLEFTHAND,
                     equipLoc ringSlot=eqLEFTRING);

        void removeItem(Item *item, equipLoc from,
                        uint8_t x=0,
                        uint8_t y=0,
                        uint8_t beltX=0);
        void dump();




    friend class Actor;



    };


}

#endif // INVENTORY_H
