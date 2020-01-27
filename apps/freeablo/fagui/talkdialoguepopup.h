#pragma once

#include "characterdialoguepopup.h"

namespace FAGui
{
    class TalkDialoguePopup : public CharacterDialoguePopup
    {
    public:
        TalkDialoguePopup(GuiManager& guiManager, const std::string& text);

        virtual UpdateResult update(struct nk_context* ctx) override;

    protected:
        virtual DialogData getDialogData() override;

    private:
        std::string mText;
    };
}
