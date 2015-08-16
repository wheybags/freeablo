#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include "item.h"
#include <map>
#include <stdint.h>
#include <diabloexe/diabloexe.h>

namespace Level
{

    class ItemPosition
    {
    public:
        ItemPosition(Item::equipLoc invType,
                     std::pair<size_t,size_t> *floorPosition,
                     std::pair<uint8_t, uint8_t> *invPosition,
                     uint8_t beltPosition);
        Item::equipLoc getInventoryType() const;
        std::pair<size_t,size_t> *getFloorPosition() const;
        uint8_t getBeltPosition() const;
        std::pair<uint8_t, uint8_t> *getInvPosition() const;

        void setInventoryType(Item::equipLoc type);
        void setFloorPosition(std::pair<size_t,size_t> * pos);
        void setBeltPosition(uint8_t x);
        void setInvPosition(std::pair<uint8_t, uint8_t> * pos);
        bool operator < (const ItemPosition rhs) const;
        bool operator==(const ItemPosition rhs) const;
    private:
        Item::equipLoc mInventoryType;
        std::pair<size_t,size_t> * mFloorPosition;
        std::pair<uint8_t, uint8_t> * mInvPosition;
        uint8_t mBeltPosition;
    };

    class ItemManager
    {
    public:
        void loadItems(DiabloExe::DiabloExe * exe);
        static bool mIsLoaded;
        void addItem(Item &item, Item::equipLoc invType, std::pair<size_t,size_t> *floorPosition, std::pair<uint8_t, uint8_t> *invPosition, uint8_t beltPosition, uint32_t count=0);
        Item getBaseItem(uint8_t id) const;
        void putItem(
                Item item,
                Item::equipLoc to,
                Item::equipLoc from,
                std::pair<uint8_t, uint8_t> * invPosition,
                uint8_t beltX,
                std::pair<size_t,size_t> * floorPosition);

        void dumpBaseItems() const;
        void dumpItemPositions();
        void removeItem(Item item);
        static uint32_t getUniqueItemCode();
        ItemManager();

    private:
        static uint32_t lastUnique;
        std::map<ItemPosition, Item> mItemPositionMap;
        static std::map<uint8_t, Item> mRegisteredItems;

    };



}



#endif // ITEMMANAGER_H
