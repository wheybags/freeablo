#pragma once
#include "../../faworld/actor.h"
#include "../characterdialoguepopup.h"

namespace FAGui
{
    class WirtDialog : public CharacterDialoguePopup
    {
    public:
        WirtDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        FAWorld::Actor* mActor = nullptr;
    };
}
