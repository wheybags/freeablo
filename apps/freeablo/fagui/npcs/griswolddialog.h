#pragma once

#include "../characterdialoguepopup.h"
#include "../../faworld/actor.h"

namespace FAGui {
    class GriswoldDialog : public CharacterDialoguePopup
    {
    public:
        GriswoldDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        void openBuyDialog();
        void openSellDialog();

        static bool griswoldSellFilter(const FAWorld::Item& item);

        FAWorld::Actor* mActor = nullptr;
    };
}
