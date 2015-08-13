#ifndef INVENTORY_H
#define INVENTORY_H
#include <stdint.h>
#include <level/item.h>
#include <diabloexe/diabloexe.h>
#include <boost/tuple/tuple.hpp>
namespace FAWorld
{
    class Actor;
    //classLevel::Item;
    class Inventory
    {
    public:
        Inventory();
        ~Inventory();
        void dump();


        Inventory testInv(Inventory & inv);
        bool putItem(Level::Item &item,
                     Level::Item::equipLoc equipType,
                     Level::Item::equipLoc from,
                     uint8_t y=0,
                     uint8_t x=0,
                     uint8_t beltX=0);
       Level::Item& getItemAt(Level::Item::equipLoc type, uint8_t y=0, uint8_t x=0, uint8_t beltX=0);
    private:
       Level::Item mInventoryBox[4][10];
       Level::Item mBelt[8];
       Level::Item mHead;
       Level::Item mBody;
       Level::Item mLeftRing;
       Level::Item mRightRing;
       Level::Item mAmulet;
       Level::Item mLeftHand;
       Level::Item mRightHand;
       Level::Item mCursorHeld;
        bool canPlaceItem(Level::Item item,
                Level::Item::equipLoc equipType,
                uint8_t y=0,
                uint8_t x=0,
                uint8_t beltX=0);
        bool fitsAt(Level::Item item, uint8_t y, uint8_t x);
        void removeItem(Level::Item &item, Level::Item::equipLoc from, uint8_t beltX=0, uint8_t invY=0, uint8_t invX=0);
    friend class Actor;
    };
}

#endif // INVENTORY_H
