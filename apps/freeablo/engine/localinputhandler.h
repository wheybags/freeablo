#pragma once
#include "../faworld/gamelevel.h"
#include "../faworld/itemmap.h"
#include "../faworld/playerinput.h"
#include "inputobserverinterface.h"
#include "misc/misc.h"
#include <input/inputmanager.h>
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
        virtual void
        notify(MouseInputAction action, Vec2i mousePosition, Vec2i mouseWheelDelta, bool mouseDown, const Input::KeyboardModifiers& modifiers) override;

        void addInput(const FAWorld::PlayerInput& input);
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
        Vec2i mLastMousePosition;
        bool mLeftMouseDown = false;
        Input::KeyboardModifiers mLastModifiers;
        bool mIsDroppingItem = false;
    };
}
