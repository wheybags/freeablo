#pragma once

#include "../characterdialoguepopup.h"
#include "../../faworld/actor.h"

namespace FAGui
{
    class OgdenDialog : public CharacterDialoguePopup
    {
    public:
        OgdenDialog(GuiManager& guiManager, FAWorld::Actor* actor);

    protected:
        virtual DialogData getDialogData() override;

        FAWorld::Actor* mActor = nullptr;
    };
}
