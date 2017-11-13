#include "dialogmanager.h"
#include <cassert>
#include "guimanager.h"

namespace FAGui
{
    DialogLineData& DialogLineData::setAction(std::function<void()> actionArg)
    {
        action = actionArg;
        return *this;
    }

    DialogLineData& DialogData::text_lines(const std::vector<std::string>& texts, TextColor color, bool alignCenter)
    {
        auto& ret = mLines[mLastLine];
        for(auto& text : texts)
        {
            mLines[mLastLine].text = text;
            mLines[mLastLine].color = color;
            mLines[mLastLine].isSeparator = false;
            mLines[mLastLine].alignCenter = alignCenter;
            ++mLastLine;
        }
        skip_line();
        return ret;
    }

    void DialogData::skip_line(int cnt)
    {
        mLastLine += cnt;
    }

    void DialogData::separator()
    {
        mLines[mLastLine].isSeparator = true;
        ++mLastLine;
        skip_line();
    }

    void DialogData::header(const std::vector<std::string>& text)
    {
        for(auto& line : text)
        {
            text_lines({line}, TextColor::golden);
        }
        separator();
        skip_line();
    }

    int DialogData::selectedLine()
    {
        if(mSelectedLine == -1)
        {
            auto it = std::find_if(mLines.begin(), mLines.end(),
                                   [](const DialogLineData& data) { return !!data.action; });
            if(it != mLines.end())
                return mSelectedLine = it - mLines.begin();
            assert(false);
        }
        return mSelectedLine;
    }

    DialogManager::DialogManager(GuiManager& gui_manager): mGuiManager(gui_manager)
    {
    }

    void DialogManager::talk(const std::string& npcId)
    {
        if(npcId == "NPCsmith")
            talkGriswold();
    }

    void DialogManager::talkGriswold()
    {
        DialogData d;
        d.header({"Welcome to the", "Blacksmith's Shop"});
        d.text_lines({"Would You Like to:"}, TextColor::golden);
        d.skip_line();
        d.text_lines({"Talk to Griswold"}, TextColor::blue).setAction([]()
        {
        });
        d.text_lines({"Buy Basic Items"}).setAction([]()
        {
        });
        d.text_lines({"Buy Premium Items"}).setAction([]()
        {
        });
        d.text_lines({"Sell Items"}).setAction([]()
        {
        });
        d.text_lines({"Repair Items"}).setAction([]()
        {
        });
        d.text_lines({"Leave the Shop"}).setAction([&]()
        { 
            mGuiManager.popDialogData ();
        });
        mGuiManager.pushDialogData(std::move (d));
    }
}
