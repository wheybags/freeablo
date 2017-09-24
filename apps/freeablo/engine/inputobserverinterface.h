#ifndef FA_INPUT_OBSERVER_INTERFACE
#define FA_INPUT_OBSERVER_INTERFACE

namespace Misc
{
    struct Point;
}

namespace Engine {

    enum KeyboardInputAction
    {
        PAUSE,
        QUIT,
        NOCLIP,
        CHANGE_LEVEL_DOWN,
        CHANGE_LEVEL_UP,
        TOGGLE_CONSOLE,
        KEYBOARD_INPUT_ACTION_MAX
    };

    enum MouseInputAction
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

#endif
