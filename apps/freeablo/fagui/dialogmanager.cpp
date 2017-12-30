#include "dialogmanager.h"
#include "../faworld/actor.h"
#include "../faworld/player.h"
#include "guimanager.h"
#include <misc/assert.h>

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
            release_assert(false);
        }
        return mSelectedLine;
    }

    void DialogData::notify(Engine::KeyboardInputAction action, GuiManager& manager)
    {
        int dir = 0;
        switch (action)
        {
            case Engine::KeyboardInputAction::nextOption:
                dir = 1;
                break;
            case Engine::KeyboardInputAction::prevOption:
                dir = -1;
                break;
            case Engine::KeyboardInputAction::reject:
                manager.popDialogData();
                return;
            case Engine::KeyboardInputAction::accept:
                mLines[mSelectedLine].action();
                return;
            default:
                break;
        }

        if (dir != 0)
        {
            int i = mSelectedLine;
            do
            {
                i += dir;
                if (i < 0)
                    i += mLines.size();
                else if (i >= static_cast<int>(mLines.size()))
                    i -= mLines.size();
            } while (!mLines[i].action);
            mSelectedLine = i;
        }
    }

    DialogManager::DialogManager(GuiManager& gui_manager, FAWorld::World& world) : mGuiManager(gui_manager), mWorld(world) {}

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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.text_lines({td.at("heal")}).setAction([&]() { mWorld.getCurrentPlayer()->heal(); });
        d.text_lines({td.at("buy")}).setAction([]() {});
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talk(const FAWorld::Actor* npc)
    {
        auto npcId = npc->getNpcId();
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
        d.text_lines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::quitDialog() const { mGuiManager.popDialogData(); }
}
