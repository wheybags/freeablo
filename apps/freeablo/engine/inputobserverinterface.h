
#pragma once

namespace Misc
{
    struct Point;
}

namespace Input
{
    struct KeyboardModifiers;
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
        toggleInventory,
        toggleQuests,
        toggleCharacterInfo,
        toggleSpellbook,
        digit0,
        digit1,
        digit2,
        digit3,
        digit4,
        digit5,
        digit6,
        digit7,
        digit8,
        digit9,
        backspace,

        max,
    };

    enum class MouseInputAction
    {
        MOUSE_RELEASE,
        MOUSE_DOWN,
        MOUSE_MOVE,
    };

    class KeyboardInputObserverInterface
    {
    public:
        virtual void notify(KeyboardInputAction action) = 0;
        virtual ~KeyboardInputObserverInterface() = default;
    };

    class MouseInputObserverInterface
    {
    public:
        virtual void notify(MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers) = 0;
        virtual ~MouseInputObserverInterface() = default;
    };
}
