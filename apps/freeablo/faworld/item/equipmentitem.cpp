#include "equipmentitem.h"
#include "equipmentitembase.h"
#include <misc/misc.h>

namespace FAWorld
{
    EquipmentItem::EquipmentItem(const EquipmentItemBase* base) : super(base) {}

    const EquipmentItemBase* EquipmentItem::getBase() const { return safe_downcast<const EquipmentItemBase*>(mBase); }
}
