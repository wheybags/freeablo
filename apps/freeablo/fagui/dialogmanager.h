
#pragma once

#include <array>
#include <functional>
#include <string>
#include <vector>

#include "../engine/inputobserverinterface.h"
#include "textcolor.h"

namespace FAWorld
{
    class World;
    class Actor;
}

namespace FAGui
{
    class GuiManager;

    class DialogLineData
    {
    public:
        DialogLineData& setAction(std::function<void()> actionArg);

    public:
        std::function<void()> action;
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
        void notify(Engine::KeyboardInputAction action, GuiManager& manager);

    private:
        std::array<DialogLineData, 24> mLines;
        int mLastLine = 1;
        int mSelectedLine = -1;
        friend class FAGui::GuiManager;
    };

    class DialogManager
    {
    public:
        explicit DialogManager(GuiManager& gui_manager, FAWorld::World& world);
        void talk(const FAWorld::Actor* npc);

    private:
        void talkOgden(const FAWorld::Actor* npc);
        void talkFarnham(const FAWorld::Actor* npc);
        void talkAdria(const FAWorld::Actor* npc);
        void talkWirt(const FAWorld::Actor* npc);
        void talkPepin(const FAWorld::Actor* npc);
        void talkCain(const FAWorld::Actor* npc);
        void talkGillian(const FAWorld::Actor* npc);
        void talkGriswold(const FAWorld::Actor* npc);
        void quitDialog() const;

    private:
        GuiManager& mGuiManager;
        FAWorld::World& mWorld;
    };
}
