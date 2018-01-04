#include "dialogmanager.h"
#include "../faworld/actor.h"
#include "../faworld/player.h"
#include "guimanager.h"
#include <misc/assert.h>
#include <utility>

namespace FAGui
{
    DialogLineData::DialogLineData(std::string text, TextColor color, bool alignCenter) : text(std::move(text)), alignCenter(alignCenter), color(color) {}

    DialogLineData::DialogLineData() {}

    DialogLineData& DialogLineData::setAction(std::function<void()> actionArg)
    {
        action = actionArg;
        return *this;
    }

    DialogLineData& DialogData::textLines(const std::vector<std::string>& texts, TextColor color, bool alignCenter)
    {
        auto index = mLines.size();
        for (auto& text : texts)
        {
            mLines.emplace_back(text, color, alignCenter);
        }
        skip_line();
        return mLines[index];
    }

    void DialogData::skip_line(int cnt) { mLines.emplace_back(); }

    void DialogData::separator()
    {
        mLines.emplace_back(DialogLineData::separator());
        skip_line();
    }

    void DialogData::footer(const std::string& text)
    {
        mFooter.emplace_back();
        mFooter.emplace_back(text, TextColor::white, true);
    }

    void DialogData::header(const std::vector<std::string>& text)
    {
        if (text.size() == 1)
        {
            mHeader.emplace_back();
            mHeader.emplace_back(text.front(), TextColor::golden, true);
            mHeader.emplace_back();
        }
        else
        {
            for (auto& line : text)
            {
                mHeader.emplace_back(line, TextColor::golden, true);
                mHeader.emplace_back();
            }
        }
        mHeader.emplace_back(DialogLineData::separator());
        mHeader.emplace_back();
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
                if (i < 0 || i >= static_cast<int>(mLines.size()))
                {
                    if (isScrollbarNeeded())
                        return; // no looping if there's scroll bar
                    i = (i + mLines.size()) % mLines.size();
                }
            } while (!mLines[i].action);
            mSelectedLine = i;
            if (!isVisible(mSelectedLine))
                if (dir == -1)
                    mFirstVisible = mSelectedLine;
                else
                {
                    int firstInvisible = mSelectedLine;
                    ++firstInvisible;
                    if (firstInvisible >= mLines.size())
                        return;
                    while (firstInvisible < mLines.size() && !mLines[firstInvisible].action)
                    {
                        ++firstInvisible;
                    }
                    mFirstVisible = firstInvisible - visibleBodyLineCount();
                }
        }
    }

    DialogManager::DialogManager(GuiManager& gui_manager, FAWorld::World& world) : mGuiManager(gui_manager), mWorld(world) {}

    void DialogManager::talkOgden(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader1"), td.at("introductionHeader2")});
        d.skip_line(2);
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(4);
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkFarnham(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});
        d.skip_line(2);
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(4);
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkAdria(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({{td.at("introductionHeader")}});

        d.skip_line(2);
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.textLines({td.at("buy")}).setAction([]() {});
        d.textLines({td.at("sell")}).setAction([]() {});
        d.textLines({td.at("recharge")}).setAction([]() {});
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkWirt(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});

        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(2);
        d.textLines({td.at("introduction1")}, TextColor::golden);
        d.textLines({td.at("introduction2")}, TextColor::golden);
        d.textLines({td.at("introduction3")}, TextColor::golden);
        d.textLines({td.at("look")}).setAction([&]() {});
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkPepin(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader1"), td.at("introductionHeader2")});

        d.skip_line(2);
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.textLines({td.at("heal")}).setAction([&]() { mWorld.getCurrentPlayer()->heal(); });
        d.textLines({td.at("buy")}).setAction([]() {});
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkCain(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});
        d.skip_line(2);
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.textLines({td.at("identify")}).setAction([]() {});
        d.skip_line(2);
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::talkGillian(const FAWorld::Actor* npc)
    {
        DialogData d;
        auto& td = npc->getTalkData();
        d.header({td.at("introductionHeader")});
        d.skip_line(2);
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.skip_line(4);
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
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
        d.textLines({td.at("introduction")}, TextColor::golden);
        d.skip_line();
        d.textLines({td.at("talk")}, TextColor::blue).setAction([]() {});
        d.textLines({td.at("buyBasic")}).setAction([]() {});
        d.textLines({td.at("buyPremium")}).setAction([]() {});
        d.textLines({td.at("sell")}).setAction([]() {});
        d.textLines({td.at("repair")}).setAction([]() {});
        d.textLines({td.at("quit")}).setAction([&]() { quitDialog(); });
        mGuiManager.pushDialogData(std::move(d));
    }

    void DialogManager::quitDialog() const { mGuiManager.popDialogData(); }
}
