#include "dialogmanager.h"
#include "../faworld/actor.h"
#include "guimanager.h"
#include <cassert>

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
        for (auto& text : texts)
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

    void DialogData::skip_line(int cnt) { mLastLine += cnt; }

    void DialogData::separator()
    {
        mLines[mLastLine].isSeparator = true;
        ++mLastLine;
        skip_line();
    }

    void DialogData::header(const std::vector<std::string>& text)
    {
        if (text.size() == 1)
        {
            skip_line();
            text_lines({text.front()}, TextColor::golden);
            skip_line();
        }
        else
        {
            for (auto& line : text)
            {
                text_lines({line}, TextColor::golden);
            }
        }
        separator();
        skip_line();
    }

    int DialogData::selectedLine()
    {
        if (mSelectedLine == -1)
        {
            auto it = std::find_if(mLines.begin(), mLines.end(), [](const DialogLineData& data) { return !!data.action; });
            if (it != mLines.end())
                return mSelectedLine = it - mLines.begin();
            assert(false);
        }
        return mSelectedLine;
    }

    DialogManager::DialogManager(GuiManager& gui_manager) : mGuiManager(gui_manager) {}

    void DialogManager::talkOgden(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader1"), td.at("introductionHeader2")});
        d.skip_line(2);
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(4);
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkFarnham(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});
        d.skip_line(2);
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(4);
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkAdria(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({{td.at("introductionHeader")}});

        d.skip_line(2);
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.text_lines({td.at("buy")}).setAction([]() {});
        d.text_lines({td.at("sell")}).setAction([]() {});
        d.text_lines({td.at("recharge")}).setAction([]() {});
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkWirt(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});

        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(2);
        d.text_lines({td.at("introduction1")}, TextColor::golden);
        d.text_lines({td.at("introduction2")}, TextColor::golden);
        d.text_lines({td.at("introduction3")}, TextColor::golden);
        d.text_lines({td.at("look")}).setAction([&]() {});
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkPepin(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader1"), td.at("introductionHeader2")});

        d.skip_line(2);
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.text_lines({td.at("heal")}).setAction([]() {});
        d.text_lines({td.at("buy")}).setAction([]() {});
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkCain(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});
        d.skip_line(2);
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.text_lines({td.at("identify")}).setAction([]() {});
        d.skip_line(2);
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkGillian(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});
        d.skip_line(2);
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(4);
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talk(const FAWorld::Actor* npc)
    {
        auto npcId = npc->getActorId();
        if (npcId == "NPCsmith")
            talkGriswold(npc);
        else if (npcId == "NPCtavern")
            talkOgden(npc);
        else if (npcId == "NPCdrunk")
            talkFarnham(npc);
        else if (npcId == "NPCmaid")
            talkGillian(npc);
        else if (npcId == "NPCboy")
            talkWirt(npc);
        else if (npcId == "NPChealer")
            talkPepin(npc);
        else if (npcId == "NPCwitch")
            talkAdria(npc);
        else if (npcId == "NPCstorytell")
            talkCain(npc);
    }

    void DialogManager::talkGriswold(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader1"), td.at("introductionHeader2")});
        d.text_lines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.text_lines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.text_lines({td.at("buyBasic")}).setAction([]() {});
        d.text_lines({td.at("buyPremium")}).setAction([]() {});
        d.text_lines({td.at("sell")}).setAction([]() {});
        d.text_lines({td.at("repair")}).setAction([]() {});
        d.text_lines({td.at("quit")}).setAction([&]() { mGuiManager.popDialogData(); });
        mGuiManager.pushDialogData(std::move(d));
    }
}
