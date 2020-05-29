#include <faworld/item/golditem.h>
#include <faworld/item/golditembase.h>

namespace FAWorld
{
    GoldItemBase::GoldItemBase(const DiabloExe::ExeItem& exeItem) : super(exeItem) { mInventoryGraphicsId = 15; }

    std::unique_ptr<Item> GoldItemBase::createItem() const { return std::unique_ptr<Item>(new GoldItem(this)); }
}