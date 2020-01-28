#pragma once

#include "../characterdialoguepopup.h"
#include "../../faworld/actor.h"

namespace FAGui
{
    class FarnhamDialog : public CharacterDialoguePopup
    {
    public:
        FarnhamDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        FAWorld::Actor* mActor = nullptr;
    };
}
