#pragma once
#include "mouseandclickmenu.h"
#include <fa_nuklear.h>
#include <vector>

namespace FAGui
{
    class GuiManager;

    class CharacterDialoguePopup
    {
    public:
        CharacterDialoguePopup(GuiManager& guiManager, bool wide);
        virtual ~CharacterDialoguePopup() = default;

        enum class UpdateResult
        {
            DoNothing,
            PopDialog
        };

        virtual UpdateResult update(struct nk_context* ctx);

        static UpdateResult actionQuit() { return UpdateResult::PopDialog; }
        static UpdateResult actionDoNothing() { return UpdateResult::DoNothing; }

    protected:
        struct DialogData
        {
            std::vector<std::string> introduction;

            std::vector<std::vector<std::string>> dialogOptions;
            std::vector<std::function<UpdateResult()>> dialogActions;

            void addMenuOption(const std::vector<std::string>& option, std::function<UpdateResult()> action)
            {
                dialogOptions.push_back(option);
                dialogActions.push_back(action);
            }
        };

        virtual DialogData getDialogData() = 0;

        GuiManager& mGuiManager;
        nk_scroll mScroll = {};
        MouseAndClickMenu mDialogMenu;
        bool mWide = false;
    };
}
