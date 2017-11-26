#pragma once
#include <memory>

struct nk_context;

namespace FARender
{
    class AnimationPlayer;
}

namespace FAGui
{
    enum class MenuFontColor
    {
        gold,
        silver,
    };


    class MainMenu
    {
    private:
        static const int menuWidth = 640;
        static const int menuHeight = 480;

    public:
        MainMenu();
        void update();
        void startingScreen(nk_context* ctx);
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        static void menuText(nk_context* ctx, const char* text, MenuFontColor color, int fontSize, uint32_t textAlignment);
    };
}
