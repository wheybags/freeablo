#pragma once
#include "item.h"

namespace FAWorld
{
    class UsableItemBase;

    class UsableItem final : public Item
    {
        using super = Item;

    public:
        explicit UsableItem(const UsableItemBase* base);

        void save(FASaveGame::GameSaver& saver) const override { super::save(saver); }
        void load(FASaveGame::GameLoader& loader) override { super::load(loader); }

        UsableItem* getAsUsableItem() override { return this; }

        const UsableItemBase* getBase() const;
    };
}
