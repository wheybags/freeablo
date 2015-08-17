#ifndef INVENTORY_H
#define INVENTORY_H
#include <stdint.h>
#include "item.h"
#include <diabloexe/diabloexe.h>
#include <map>
namespace FAWorld
{

    class Actor;

    class Inventory
    {
    public:
        Inventory(Actor * actor);

        void dump();


        Inventory testInv(Inventory & inv);
        void collectEffects();

        bool putItem(Item &item,
                     Item::equipLoc equipType,
                     Item::equipLoc from,
                     uint8_t y=0,
                     uint8_t x=0,
                     uint8_t beltX=0);
       Item& getItemAt(Item::equipLoc type, uint8_t y=0, uint8_t x=0, uint8_t beltX=0);

    private:

       Item mInventoryBox[4][10];
       Item mBelt[8];
       Item mHead;
       Item mBody;
       Item mLeftRing;
       Item mRightRing;
       Item mAmulet;
       Item mLeftHand;
       Item mRightHand;
       Item mCursorHeld;
       uint32_t mArmourClassTotal;
       uint32_t mAttackDamageTotal;
       Actor * mActor;
       std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> mItemEffects;

       bool canPlaceItem(Item item,
                         Item::equipLoc equipType,
                         uint8_t y=0,
                         uint8_t x=0,
                         uint8_t beltX=0);
       bool checkStatsRequirement(Item& item);
       bool fitsAt(Item item, uint8_t y, uint8_t x);
       void removeItem(Item &item, Item::equipLoc from, uint8_t beltX=0, uint8_t invY=0, uint8_t invX=0);
    friend class Actor;

    };
}

#endif // INVENTORY_H
