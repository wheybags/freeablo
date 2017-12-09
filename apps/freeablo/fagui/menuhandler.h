#pragma once
#include <memory>

namespace Engine
{
    class EngineMain;
}

struct nk_context;

namespace FARender
{
    class AnimationPlayer;
}

namespace FAGui
{
    class MenuHandler;

    enum class MenuFontColor
    {
        gold,
        silver,
    };

    class MenuScreen
    {
    public:
        explicit MenuScreen(MenuHandler& menu);
        virtual ~MenuScreen();
        virtual void update(nk_context* ctx) = 0;

    protected:
        static void menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment);

    protected:
        MenuHandler& mMenuHandler;
    };

    class StartingScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;

    public:
        explicit StartingScreen(MenuHandler& menu);
        void menuItems(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        int activeItemIndex = 0;
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus42;
    };

    class MenuHandler
    {
    public:
        static const int width = 640;
        static const int height = 480;
        std::unique_ptr<MenuScreen> mActiveScreen;
        template <typename ScreenType> void setActiveScreen() { mActiveScreen.reset(new ScreenType(*this)); }

    public:
        explicit MenuHandler(Engine::EngineMain& engine);
        void update(nk_context* ctx) const;
        void quit();
        void startGame();

    private:
        Engine::EngineMain& mEngine;
    };
}
