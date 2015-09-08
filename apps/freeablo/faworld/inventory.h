#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdint.h>
#include "item.h"
#include <diabloexe/diabloexe.h>
#include <map>

namespace FAWorld
{
    class Player;
    class CharacterStatsBase;

    class Inventory
    {
    public:
        Inventory(Player * actor);

        void dump();



        void collectEffects();

        bool putItem(Item &item,
                     Item::equipLoc equipType,
                     Item::equipLoc from,
                     uint8_t y=0,
                     uint8_t x=0,
                     uint8_t beltX=0,
                     bool recalculateStats=true);
       Item& getItemAt(Item::equipLoc type, uint8_t y=0, uint8_t x=0, uint8_t beltX=0);
       std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t> > & getTotalEffects();
       bool canPlaceItem(Item item,
                         Item::equipLoc equipType,
                         uint8_t y=0,
                         uint8_t x=0,
                         uint8_t beltX=0);
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
       Player * mActor;
       std::vector<std::tuple<Item::ItemEffect, uint32_t, uint32_t, uint32_t>> mItemEffects;
       bool checkStatsRequirement(Item& item);
       bool fitsAt(Item item, uint8_t y, uint8_t x);
       void removeItem(Item &item, Item::equipLoc from, uint8_t beltX=0, uint8_t invY=0, uint8_t invX=0);
       static const uint8_t GOLD_PILE_MIN=15;
       static const uint8_t GOLD_PILE_MID=16;
       static const uint8_t GOLD_PILE_MAX=17;
    friend class Player;
    friend class CharacterStatsBase;

    };
}

#endif // INVENTORY_H
