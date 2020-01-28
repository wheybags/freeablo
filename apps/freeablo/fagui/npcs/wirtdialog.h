#pragma once

#include "../characterdialoguepopup.h"
#include "../../faworld/actor.h"

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
