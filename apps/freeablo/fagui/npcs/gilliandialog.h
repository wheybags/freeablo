#pragma once
#include "../../faworld/actor.h"
#include "../characterdialoguepopup.h"

namespace FAGui
{
    class GillianDialog : public CharacterDialoguePopup
    {
    public:
        GillianDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        FAWorld::Actor* mActor = nullptr;
    };
}
