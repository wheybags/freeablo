#include "usableitem.h"
#include "usableitembase.h"
#include <engine/threadmanager.h>
#include <misc/misc.h>

namespace FAWorld
{
    UsableItem::UsableItem(const UsableItemBase* base) : super(base) {}

    const UsableItemBase* UsableItem::getBase() const { return safe_downcast<const UsableItemBase*>(mBase); }

    void UsableItem::applyEffect(Player& user)
    {
        if (!getBase()->mUseSoundPath.empty())
            Engine::ThreadManager::get()->playSound(getBase()->mUseSoundPath);

        getBase()->mEffect(user);
    }
}
