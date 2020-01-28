#pragma once

#include "../characterdialoguepopup.h"
#include "../../faworld/actor.h"

namespace FAGui
{
    class CainDialog : public CharacterDialoguePopup
    {
    public:
        CainDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        FAWorld::Actor* mActor = nullptr;
    };
}
