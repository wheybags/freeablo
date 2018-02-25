#pragma once
#include "../faworld/gamelevel.h"
#include "../faworld/itemmap.h"
#include "../faworld/playerinput.h"
#include "inputobserverinterface.h"
#include "misc/misc.h"
#include <vector>

namespace FAWorld
{
    class World;
}

namespace Engine
{
    class LocalInputHandler : public MouseInputObserverInterface, public KeyboardInputObserverInterface
    {
    public:
        LocalInputHandler(FAWorld::World& world);
        virtual ~LocalInputHandler() = default;

        virtual void notify(KeyboardInputAction action) override;
        virtual void notify(MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers) override;

        std::vector<FAWorld::PlayerInput> getAndClearInputs();

        void blockInput();
        void unblockInput();

        void update();

        const FAWorld::HoverStatus& getHoverStatus() { return mHoverStatus; }

    private:
        FAWorld::World& mWorld;
        std::vector<FAWorld::PlayerInput> mInputs;
        int32_t mBlockedFramesLeft = 0;
        bool mUnblockInput = true;

        FAWorld::HoverStatus mHoverStatus;
    };
}
