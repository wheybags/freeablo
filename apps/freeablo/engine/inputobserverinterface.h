
#pragma once

namespace Misc
{
    struct Point;
}

namespace Engine
{

    enum class KeyboardInputAction
    {
        pause,
        quit,
        noclip,
        changeLevelDown,
        changeLevelUp,
        toggleConsole,
        nextOption,
        prevOption,
        accept,
        reject,

        max
    };

    enum class MouseInputAction
    {
        MOUSE_RELEASE,
        MOUSE_DOWN,
        MOUSE_CLICK,
        MOUSE_MOVE,
    };

    class KeyboardInputObserverInterface
    {
    public:
        virtual void notify(KeyboardInputAction action) = 0;
    };

    class MouseInputObserverInterface
    {
    public:
        virtual void notify(MouseInputAction action, Misc::Point mousePosition) = 0;
    };
}
