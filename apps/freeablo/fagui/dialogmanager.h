#pragma once
#include <array>
#include <functional>
#include <string>
#include <vector>
#include "../engine/inputobserverinterface.h"
#include "../farender/spritecache.h"
#include "textcolor.h"
#include <boost/optional.hpp>
#include <memory>

namespace FAWorld
{
    class World;
    class Actor;
    class Item;
    struct StoreItem;
}

namespace FAGui
{
    class GuiManager;
    class CharacterDialoguePopup;

    class DialogLineData
    {
    public:
        DialogLineData(std::string text, TextColor color, bool alignCenter);
        DialogLineData();
        DialogLineData& setAction(std::function<void()> actionArg);
        DialogLineData& setNumber(int32_t number);
        DialogLineData& setYOffset(int32_t offset);
        DialogLineData& setXOffset(int32_t offset);
        DialogLineData& setForceSelected(bool value = true)
        {
            forceSelected = value;
            return *this;
        }

        static DialogLineData separator()
        {
            DialogLineData data;
            data.isSeparator = true;
            return data;
        }

    public:
        std::function<void()> action;
        std::string text;
        bool alignCenter = false;
        bool isSeparator = false;
        bool forceSelected = false; /// display selection forcefully on this line (Used for back button in some cases)
        TextColor color = TextColor::white;
        int32_t mXOffset = 0;
        int32_t mYOffset = 0;
        boost::optional<int32_t> mNumber;
    };

    class DialogData
    {
        static constexpr int32_t linesVisible = 24;

    public:
        static DialogLineData toLineData(const std::string& text, TextColor color, bool alignCenter);
        DialogLineData&
        textLines(const std::vector<std::string>& texts, TextColor color = TextColor::white, bool alignCenter = true, std::vector<int32_t> offsets = {});
        void skip_line(int32_t cnt = 1);
        void separator();
        DialogLineData& footer(const std::string& text);
        void header(const std::vector<std::string>& text);
        void header(std::vector<DialogLineData> data) { mHeader = std::move(data); }
        int32_t selectedLine();
        void notify(Engine::KeyboardInputAction action, GuiManager& manager);
        void widen() { mIsWide = true; }
        void showScrollBar(bool show = true) { mScrollBarShown = show; }
        bool isScrollbarShown() const { return mScrollBarShown; }
        int32_t visibleBodyLineCount() const { return linesVisible - mHeader.size() - mFooter.size(); }
        bool isVisible(int32_t line) const { return line >= mFirstVisible && line < mFirstVisible + visibleBodyLineCount(); }
        double selectedLinePercent();
        void clearLines();
        std::vector<DialogLineData> getHeader() const { return mHeader; }

        // HACK
        struct
        {
            const FAWorld::Actor* shopkeeper = nullptr;
            std::vector<FAWorld::StoreItem>* items = nullptr;
            uint32_t itemId = 0;
        } shopData;

    private:
        std::vector<DialogLineData> mHeader;
        std::vector<DialogLineData> mLines;
        std::vector<DialogLineData> mFooter;
        int mSelectedLine = -1; // -1 means the first selectable
        int mFirstVisible = 0;  // used in case if not all fit to the window
        bool mIsWide = false;
        friend class FAGui::GuiManager;
        bool mScrollBarShown = false;
    };



    class DialogManager
    {
    public:
        explicit DialogManager(GuiManager& gui_manager, FAWorld::World& world);
        ~DialogManager();

        void talk(const FAWorld::Actor* npc);
        void fillMessageDialog(std::vector<DialogLineData> header, const char* text);

        void update(struct nk_context* ctx);

    private:
        template <typename FilterType> void sellDialog(FilterType filter);
        void buyDialog(const FAWorld::Actor* shopkeeper, std::vector<FAWorld::StoreItem>& items);

        void talkOgden(const FAWorld::Actor* npc);
        void talkFarnham(const FAWorld::Actor* npc);
        void talkAdria(const FAWorld::Actor* npc);
        void talkWirt(const FAWorld::Actor* npc);
        void talkPepin(const FAWorld::Actor* npc);
        void talkCain(const FAWorld::Actor* npc);
        void talkGillian(const FAWorld::Actor* npc);
        void talkGriswold(const FAWorld::Actor* npc);
        void quitDialog() const;

        void confirmDialog(std::vector<DialogLineData> header, const FAWorld::Item& item, int price, const char* question, std::function<void()> successAction);

    public:
        GuiManager& mGuiManager;
//    private:
        FAWorld::World& mWorld;
        std::vector<std::unique_ptr<CharacterDialoguePopup>> mDialogStack;
    };
}
