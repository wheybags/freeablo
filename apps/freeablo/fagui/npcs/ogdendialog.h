#pragma once
#include "../../faworld/actor.h"
#include "../characterdialoguepopup.h"

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
