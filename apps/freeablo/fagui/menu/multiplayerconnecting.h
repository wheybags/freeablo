#pragma once
#include "menuscreen.h"

namespace FAGui
{
    class MultiplayerConnecting : public MenuScreen
    {
    public:
        explicit MultiplayerConnecting(MenuHandler& menu);
        void update(nk_context* ctx) override;

    private:
        bool content(nk_context* ctx);

    private:
        enum class State
        {
            Connecting,
            ConnectionFailed
        };

        State mState = State::Connecting;
    };
}
