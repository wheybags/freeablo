#ifndef ITEM_H
#define ITEM_H
#include "inventory.h"
#include <diabloexe/baseitem.h>
#include <diabloexe/prefix.h>
namespace FAWorld
{
    class Item
        {
        public:
            bool inline isEmpty(){return mEmpty;}
            Item():mEmpty(true)
            {}

        private:
            DiabloExe::BaseItem mItem;
            DiabloExe::Prefix mPrefix;
            uint8_t mSizeX;
            uint8_t mSizeY;
            uint8_t mInvX;
            uint8_t mInvY;
            uint8_t mBeltX;
            bool mEmpty;

        friend class Inventory;
        };
}
#endif // ITEM_H
