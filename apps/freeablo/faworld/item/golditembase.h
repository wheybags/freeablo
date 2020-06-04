#pragma once
#include "itembase.h"

namespace FAWorld
{
    class GoldItemBase final : public ItemBase
    {
        using super = ItemBase;

    public:
        explicit GoldItemBase(const DiabloExe::ExeItem& exeItem);
        ~GoldItemBase() override;

        std::unique_ptr<Item> createItem() const override;

        const Render::TextureReference* mInventoryIcon2 = nullptr;
        std::unique_ptr<Render::Cursor> mInventoryIconCursor2;

        const Render::TextureReference* mInventoryIcon3 = nullptr;
        std::unique_ptr<Render::Cursor> mInventoryIconCursor3;

    public:
        int32_t mMaxCount = 5000;
    };
}
