#include "miscitem.h"
#include "miscitembase.h"
#include <misc/misc.h>

namespace FAWorld
{
    MiscItem::MiscItem(const MiscItemBase* base) : super(base) {}

    const MiscItemBase* MiscItem::getBase() const { return safe_downcast<const MiscItemBase*>(mBase); }
}
