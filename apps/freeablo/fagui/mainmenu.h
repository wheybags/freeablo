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
    class MainMenuHandler;

    enum class MenuFontColor
    {
        gold,
        silver,
    };

    class MainMenuScreen
    {
    public:
        explicit MainMenuScreen(MainMenuHandler& menu);
        virtual ~MainMenuScreen();
        virtual void update(nk_context* ctx) = 0;

    protected:
        static void menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment);

    protected:
        MainMenuHandler& mMenuHandler;
    };

    class StartingScreen : public MainMenuScreen
    {
    private:
        using Parent = MainMenuScreen;

    public:
        explicit StartingScreen(MainMenuHandler& menu);
        void menuItems(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        int activeItemIndex = 0;
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus42;
    };

    class MainMenuHandler
    {
    public:
        static const int width = 640;
        static const int height = 480;
        std::unique_ptr<MainMenuScreen> mActiveScreen;
        template <typename ScreenType> void setActiveScreen() { mActiveScreen.reset(new ScreenType(*this)); }

    public:
        explicit MainMenuHandler(Engine::EngineMain& engine);
        void update(nk_context* ctx) const;
        void quit();
        void startGame();

    private:
        Engine::EngineMain& mEngine;
    };
}
