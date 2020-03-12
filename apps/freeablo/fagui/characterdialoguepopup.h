#pragma once
#include "menuentry.h"
#include "mouseandclickmenu.h"
#include <fa_nuklear.h>
#include <vector>

namespace FAGui
{
    class GuiManager;

    class CharacterDialoguePopup
    {
    public:
        CharacterDialoguePopup(GuiManager& guiManager, bool wide, const std::string& greeting = "");
        virtual ~CharacterDialoguePopup() = default;

        enum class UpdateResult
        {
            DoNothing,
            PopDialog
        };

        virtual UpdateResult update(struct nk_context* ctx);

        static UpdateResult actionQuit() { return UpdateResult::PopDialog; }
        static UpdateResult actionDoNothing() { return UpdateResult::DoNothing; }

        const std::string& getGreetingPath() const { return mSoundPaths.at("greeting"); }

    protected:
        struct DialogData
        {
            std::vector<MenuEntry> introduction;

            std::vector<std::vector<MenuEntry>> dialogOptions;
            std::vector<std::function<UpdateResult()>> dialogActions;

            void addMenuOption(const std::vector<MenuEntry>& option, std::function<UpdateResult()> action)
            {
                dialogOptions.push_back(option);
                dialogActions.push_back(action);
            }
        };

        virtual DialogData getDialogData() = 0;
        void openTalkDialog(const FAWorld::Actor* actor);
        void drawBackgroundCheckerboard(FARender::Renderer* renderer, struct nk_context* ctx, struct nk_rect dialogRectangle);

        GuiManager& mGuiManager;
        nk_scroll mScroll = {0, 0};
        MouseAndClickMenu mDialogMenu;
        bool mWide = false;
        std::unordered_map<std::string, std::string> mSoundPaths;
    };
}
