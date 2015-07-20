#ifndef ITEM_H
#define ITEM_H
#include <diabloexe/baseitem.h>
#include <diabloexe/prefix.h>
#include <render/render.h>
//#include "inventory.h"
namespace FAWorld
{
        class Inventory;
}
namespace Level
{
    //class Inventory;


    class ItemPosition;
    class Item
        {
        friend class FAWorld::Inventory;
        friend class ItemPosition;
        public:
            bool inline isEmpty(){return mEmpty;}
            Item();
            ~Item();
            Item(DiabloExe::BaseItem item, size_t id);
            std::string getName() const;
            void setUniqueId(uint32_t mUniqueId);
            uint32_t getUniqueId() const;
            void setCount(uint32_t count);
            uint32_t getCount() const;
            bool operator==(const Item rhs) const;
            std::pair<uint8_t, uint8_t> getInvSize() const;
            DiabloExe::BaseItem mItem;
            uint8_t mInvY;
            uint8_t mInvX;
            uint8_t mBeltX;
            bool mIsReal;
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
            static Cel::CelFile * mObjcurs;
            DiabloExe::Prefix mPrefix;
            uint8_t mSizeX;
            uint8_t mSizeY;
            uint8_t mBaseId;
            uint32_t mUniqueId;
            uint32_t mCount;
            uint32_t mMaxCount;
            bool mEmpty;
            static bool mObjcursLoaded;

        };
}
#endif // ITEM_H
