#pragma once
#include <diabloexe/baseitem.h>
#include <misc/simplevec2.h>

namespace FAWorld
{
    class ItemBase
    {
    protected:
        explicit ItemBase(const DiabloExe::ExeItem& exeItem);

    public:
        virtual ~ItemBase() = default;

    public:
        std::string mId;
        ItemType mType = ItemType::none;

        std::string mName;
        std::string mShortName;

        Vec2i mSize;

        std::string mDropItemGraphicsPath;
        int32_t mInventoryGraphicsId = -1;

        std::string mDropItemSoundPath;
        std::string mInventoryPlaceItemSoundPath;
    };
}
