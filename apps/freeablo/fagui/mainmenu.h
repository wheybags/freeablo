#pragma once
#include <memory>

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
        explicit MainMenuScreen(const MainMenuHandler& menu);
        virtual ~MainMenuScreen();
        virtual void update(nk_context* ctx) = 0;

    protected:
        static void menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment);

    protected:
        const MainMenuHandler& mMenu;
    };

    class StartingScreen : public MainMenuScreen
    {
    private:
        using Parent = MainMenuScreen;
    public:
        explicit StartingScreen(const MainMenuHandler& menu);
        void update(nk_context* ctx) override;

    private:
        int active_item_index = 0;
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus42;
    };

    class MainMenuHandler
    {
    public:
        static const int width = 640;
        static const int height = 480;
        std::unique_ptr<MainMenuScreen> mActiveScreen;
        template <typename ScreenType>
        void setActiveScreen ()
        {
            mActiveScreen.reset (new ScreenType(*this));
        }

    public:
        MainMenuHandler();
        void update(nk_context* ctx) const;
    };
}
