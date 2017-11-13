#pragma once
#include <array>
#include <string>
#include <functional>
#include <vector>

#include "textcolor.h"

namespace FAGui
{
    class GuiManager;

    class DialogLineData
    {
    public:
        DialogLineData& setAction(std::function<void ()> actionArg);

    public:
        std::function<void ()> action;
        std::string text;
        bool alignCenter = false;
        bool isSeparator = false;
        TextColor color = TextColor::white;
    };

    class DialogData
    {
    public:
        DialogLineData& text_lines(const std::vector<std::string>& texts, TextColor color = TextColor::white, bool alignCenter = true);
        void skip_line(int cnt = 1);
        void separator();
        void header(const std::vector<std::string>& text);
        int selectedLine();

    private:
        std::array<DialogLineData, 24> mLines;
        int mLastLine = 1;
        int mSelectedLine = -1;
        friend class FAGui::GuiManager;
    };

    class DialogManager
    {
    public:
        explicit DialogManager(GuiManager& gui_manager);
        void talk(const std::string& npcId);

    private:
        void talkGriswold();

    private:
        GuiManager& mGuiManager;
    };
}
