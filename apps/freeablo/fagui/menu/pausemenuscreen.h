#pragma once
#include "menuscreen.h"
#include <cstdint>
#include <memory>

struct nk_context;
typedef uint32_t nk_flags;

namespace FARender
{
    class AnimationPlayer;
}

namespace FAGui
{
    class MenuHandler;

    class PauseMenuScreen : public MenuScreen
    {
    private:
        using Parent = MenuScreen;

    public:
        explicit PauseMenuScreen(FAGui::MenuHandler& menu);
        static void bigTGoldText(nk_context* ctx, const char* text, nk_flags alignment);
        static float bigTGoldTextWidth(const char* text);
        void menuItems(nk_context* ctx);
        void update(nk_context* ctx) override;

    private:
        std::unique_ptr<FARender::AnimationPlayer> mBigPentagram;
    };
}
