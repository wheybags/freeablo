#include "usableitem.h"
#include "usableitembase.h"
#include <misc/misc.h>

namespace FAWorld
{
    UsableItem::UsableItem(const UsableItemBase* base) : super(base) {}

    const UsableItemBase* UsableItem::getBase() const { return safe_downcast<const UsableItemBase*>(mBase); }
}
