#pragma once
#include "menuscreen.h"
#include <memory>

namespace FARender
{
    class AnimationPlayer;
}

namespace FAGui
{
    class StartingMenuScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;

    public:
        explicit StartingMenuScreen(MenuHandler& menu);
        void menuItems(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        std::unique_ptr<FARender::AnimationPlayer> mSmLogo;
        std::unique_ptr<FARender::AnimationPlayer> mFocus42;
    };
}
