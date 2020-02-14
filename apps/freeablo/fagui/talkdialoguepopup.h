#pragma once
#include "../components/diabloexe/talkdata.h"
#include "characterdialoguepopup.h"

namespace FAGui
{
    class TalkDialoguePopup : public CharacterDialoguePopup
    {
    public:
        TalkDialoguePopup(GuiManager& guiManager, DiabloExe::TalkData talk);

        virtual UpdateResult update(struct nk_context* ctx) override;

        const DiabloExe::TalkData& getTalkData() const;

    protected:
        virtual DialogData getDialogData() override;

    private:
        DiabloExe::TalkData mTalkData;
    };
}
