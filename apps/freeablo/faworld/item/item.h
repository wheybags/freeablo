#pragma once
#include <cstdint>
#include <fagui/menuentry.h>
#include <misc/misc.h>
#include <string>
#include <vector>

namespace FASaveGame
{
    class GameSaver;
    class GameLoader;
}

namespace FAWorld
{
    class EquipmentItem;
    class UsableItem;

    class ItemBase;

    class Item2
    {
    public:
        explicit Item2(const ItemBase* base) : mBase(base) {}

        virtual void init() {}

        virtual void save(FASaveGame::GameSaver& saver) const { UNUSED_PARAM(saver); }
        virtual void load(FASaveGame::GameLoader& loader) { UNUSED_PARAM(loader); }

        virtual EquipmentItem* getAsEquipmentItem() { return nullptr; }
        const EquipmentItem* getAsEquipmentItem() const { return const_cast<Item2*>(this)->getAsEquipmentItem(); }

        virtual UsableItem* getAsUsableItem() { return nullptr; }
        const UsableItem* getAsMiscItem() const { return const_cast<Item2*>(this)->getAsUsableItem(); }

        int32_t getPrice() const;
        virtual std::string getFullDescription() const;
        virtual std::vector<FAGui::MenuEntry> descriptionForMerchants() const;

        const ItemBase* getBase() const { return mBase; }

    protected:
        const ItemBase* mBase = nullptr;
    };
}
