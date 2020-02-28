#pragma once
#include <misc/simplevec2.h>

namespace Input
{
    struct KeyboardModifiers;
    class Hotkey;
}

namespace Engine
{

    enum class KeyboardInputAction
    {
        pause,
        quit,
        noclip,
        dataChangeLevelDown,
        dataChangeLevelUp,
        toggleConsole,
        nextOption,
        prevOption,
        accept,
        reject,
        toggleInventory,
        toggleQuests,
        toggleCharacterInfo,
        toggleSpellbook,
        toggleSpellSelection,
        spellHotkeyF5,
        spellHotkeyF6,
        spellHotkeyF7,
        spellHotkeyF8,

        max,
    };

    enum class MouseInputAction
    {
        MOUSE_RELEASE,
        MOUSE_DOWN,
        RIGHT_MOUSE_RELEASE,
        RIGHT_MOUSE_DOWN,
        MOUSE_MOVE,
    };

    class KeyboardInputObserverInterface
    {
    public:
        virtual void notify(KeyboardInputAction action) = 0;
        virtual ~KeyboardInputObserverInterface() = default;
        /// should be used for handling text input
        virtual void keyPress(const Input::Hotkey&) {}
    };

    class MouseInputObserverInterface
    {
    public:
        virtual void notify(MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers) = 0;
        virtual ~MouseInputObserverInterface() = default;
    };
}
