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
            Item(){};

        private:
            DiabloExe::BaseItem item;
            DiabloExe::Prefix prefix;
            uint8_t sizeX;
            uint8_t sizeY;
            uint8_t invX;
            uint8_t invY;

        friend class Inventory;
        }const static emptyItem;
}
#endif // ITEM_H
