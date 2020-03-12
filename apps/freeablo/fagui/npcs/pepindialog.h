#pragma once
#include "../../faworld/actor.h"
#include "../characterdialoguepopup.h"

namespace FAGui
{
    class PepinDialog : public CharacterDialoguePopup
    {
    public:
        PepinDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        FAWorld::Actor* mActor = nullptr;
    };
}
