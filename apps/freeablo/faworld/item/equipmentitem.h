#pragma once
#include "item.h"

namespace FAWorld
{
    class EquipmentItemBase;

    class EquipmentItem final : public Item2
    {
        using super = Item2;

    public:
        explicit EquipmentItem(const EquipmentItemBase* base);

        void init() override;

        void save(FASaveGame::GameSaver& saver) const override;
        void load(FASaveGame::GameLoader& loader) override;

        EquipmentItem* getAsEquipmentItem() override { return this; }
        const EquipmentItemBase* getBase() const;

        std::string getFullDescription() const override;

    public:
        int32_t mArmorClass = 0;
    };
}
