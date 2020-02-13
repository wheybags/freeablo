#pragma once
#include "menuscreen.h"
#include <memory>
#include <optional>

namespace FAGui
{
    class MultiplayerMenu : public MenuScreen
    {
    private:
        using Parent = MenuScreen;

    public:
        explicit MultiplayerMenu(MenuHandler& menu);
        void update(nk_context* ctx) override;

    private:
        bool content(nk_context* ctx);

    private:
        static constexpr size_t IP_BUFFER_SIZE = 2048;
        char mIpBuffer[IP_BUFFER_SIZE] = {0};
    };
}
