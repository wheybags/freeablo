#ifndef ITEM_H
#define ITEM_H
#include <diabloexe/baseitem.h>
#include <diabloexe/prefix.h>
//#include "inventory.h"
namespace FAWorld
{
    //class Inventory;
    class Inventory;
    class ItemPosition;
    class Item
        {
        friend class Inventory;
        friend class ItemPosition;
        public:
            bool inline isEmpty(){return mEmpty;}
            Item();
            Item(DiabloExe::BaseItem item, size_t id);
            std::string getName();

        private:
            DiabloExe::BaseItem mItem;
            DiabloExe::Prefix mPrefix;
            uint8_t mSizeX;
            uint8_t mSizeY;
            uint8_t mInvX;
            uint8_t mInvY;
            uint8_t mBeltX;
            uint8_t mFloorX;
            uint8_t mFloorY;
            uint8_t mBaseId;
            uint8_t mUniqueId;
            uint32_t mCount;
            uint32_t mMaxCount;
            bool mEmpty;

        };
}
#endif // ITEM_H
