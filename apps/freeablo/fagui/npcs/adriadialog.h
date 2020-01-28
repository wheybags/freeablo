#pragma once

#include "../characterdialoguepopup.h"
#include "../../faworld/actor.h"
#include "../../faworld/item.h"

namespace FAGui
{
    class AdriaDialog : public CharacterDialoguePopup
    {
    public:
        AdriaDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        void openSellDialog();
        static bool adriaSellFilter(const FAWorld::Item& item);

        FAWorld::Actor* mActor = nullptr;
    };
}
