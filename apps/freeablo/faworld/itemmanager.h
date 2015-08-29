#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include "item.h"
#include <map>
#include <stdint.h>
#include <diabloexe/diabloexe.h>
#include <level/baseitemmanager.h>
#include <boost/program_options.hpp>

namespace Engine
{
    class EngineMain;
}
namespace FAWorld

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

    class ItemManager : public Level::BaseItemManager
    {
    public:
        static bool mIsLoaded;
        void addItem(Item &item, std::pair<size_t, size_t> floorPosition, uint32_t count);
        Item getBaseItem(uint8_t id) const;
        Item getUniqueItem(uint8_t id) const;
        DiabloExe::BaseItem &getBaseItemByUniqueCode(uint8_t uniqCode);
        void putItemOnFloor(Item& item, std::pair<size_t, size_t> floor_pos);
        void dumpBaseItems() const;
        void dumpUniqueItems() const;
        void dumpItemPositions();
        void removeItem(Item item);
        static uint32_t getUniqueItemCode();
        ItemManager();
    private:
        void loadItems(DiabloExe::DiabloExe * exe);
        void loadUniqueItems(DiabloExe::DiabloExe * exe);
        std::map<ItemPosition, Item> mItemPositionMap;
        static std::map<uint8_t, Item> mRegisteredItems;
        static std::map<uint8_t, Item> mUniqueItems;
        static std::map<uint32_t, DiabloExe::BaseItem> mUniqueCodeToBaseItem;
        static uint32_t lastUnique;
        friend class Engine::EngineMain;

    };
}



#endif // ITEMMANAGER_H
