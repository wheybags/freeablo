#pragma once
#include "../../faworld/actor.h"
#include "../characterdialoguepopup.h"

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
