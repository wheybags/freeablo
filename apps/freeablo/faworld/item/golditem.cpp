#include <faworld/item/golditem.h>
#include <faworld/item/golditembase.h>
#include <fmt/format.h>

namespace FAWorld
{
    GoldItem::GoldItem(const GoldItemBase* base) : super(base) {}

    const GoldItemBase* GoldItem::getBase() const { return safe_downcast<const GoldItemBase*>(mBase); }

    bool GoldItem::trySetCount(int32_t newCount)
    {
        if (newCount < 1 || newCount > getBase()->mMaxCount)
            return false;

        mCount = newCount;
        return true;
    }

    std::string GoldItem::getFullDescription() const { return fmt::format("{} {} {}", mCount, getBase()->mName, mCount > 1 ? "pieces" : "piece"); }

    const Render::TextureReference* GoldItem::getInventoryIcon() const
    {
        if (mCount <= 1000)
            return getBase()->mInventoryIcon;
        if (mCount <= 2500)
            return getBase()->mInventoryIcon2;

        return getBase()->mInventoryIcon3;
    }

    const Render::Cursor* GoldItem::getInventoryIconCursor() const
    {
        if (mCount <= 1000)
            return getBase()->mInventoryIconCursor.get();
        if (mCount <= 2500)
            return getBase()->mInventoryIconCursor2.get();

        return getBase()->mInventoryIconCursor3.get();
    }
}