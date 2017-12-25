
#pragma once

#include <functional>
#include <vector>

struct nk_context;

namespace Engine
{
    enum class KeyboardInputAction;
}

namespace FAGui
{
    class MenuHandler;
    enum class MenuFontColor;

    class MenuScreen
    {
    protected:
        enum class DrawFunctionResult
        {
            executeAction,
            setActive,
            noAction,
        };
        enum class ActionResult
        {
            stopDrawing,
            continueDrawing,
        };
        class MenuItem
        {
        public:
            std::function<DrawFunctionResult(nk_context* ctx, bool isActive)> drawFunction;
            std::function<ActionResult()> action;
        };

    public:
        explicit MenuScreen(MenuHandler& menu);
        virtual ~MenuScreen();
        virtual void update(nk_context* ctx) = 0;
        void notify(Engine::KeyboardInputAction action);

    protected:
        static void menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment);
        ActionResult drawMenuItems(nk_context* ctx);
        ActionResult executeActive();

    protected:
        MenuHandler& mMenuHandler;
        std::function<ActionResult()> mRejectAction;
        std::vector<MenuItem> mMenuItems;
        int mActiveItemIndex = 0;
    };
}
