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
        ItemPosition(){};
        ItemPosition(std::pair<size_t, size_t> floorPosition);
        void setFloorPosition(std::pair<size_t,size_t> pos);
        std::pair<size_t, size_t> getFloorPosition() const;
        bool operator < (const ItemPosition rhs) const;
        bool operator==(const ItemPosition rhs) const;
    private:        
        std::pair<size_t,size_t> mFloorPosition;
    };




    class ItemManager
    {
    public:
        void loadItems(DiabloExe::DiabloExe * exe);
        static bool mIsLoaded;
        void addItem(Item &item, std::pair<size_t, size_t> floorPosition, uint32_t count);
        Item getBaseItem(uint8_t id) const;
        void putItemOnFloor(Item& item, std::pair<size_t, size_t> floor_pos);

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
