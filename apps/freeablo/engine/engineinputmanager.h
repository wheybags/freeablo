#pragma once
#include "../components/misc/misc.h"
#include "../faworld/actoranimationmanager.h"
#include "inputobserverinterface.h"
#include <fa_nuklear.h>
#include <input/hotkey.h>
#include <input/inputmanager.h>
#include <map>
#include <string>
#include <vector>

namespace FAWorld
{
    class World;
}

namespace FAGui
{
    class GuiManager;
}

namespace Input
{
    struct KeyboardModifiers;
}

namespace Engine
{
    class EngineMain;

    class EngineInputManager
    {
    public:
        EngineInputManager(nk_context* nk_ctx);
        void update(bool paused);
        void setHotkey(KeyboardInputAction action, Input::Hotkey hotkey);
        Input::Hotkey getHotkey(KeyboardInputAction action);
        std::vector<Input::Hotkey> getHotkeys();
        void registerKeyboardObserver(KeyboardInputObserverInterface* observer);
        void registerMouseObserver(MouseInputObserverInterface* observer);
        void setGuiManager(FAGui::GuiManager* guiManager);
        Input::KeyboardModifiers getKeyboardModifiers() { return mKbMods; }

    private:
        EngineInputManager(const EngineInputManager&);
        EngineInputManager& operator=(const EngineInputManager&);
        void keyPress(Input::Key key);
        void keyRelease(Input::Key key);
        void textInput(std::string inp);
        void mouseClick(int32_t x, int32_t y, Input::Key key, bool isDoubleClick);
        void mouseRelease(int32_t, int32_t, Input::Key key);
        void mouseMove(int32_t x, int32_t y, int32_t xrel, int32_t yrel);
        std::string keyboardActionToString(KeyboardInputAction action) const;
        void notifyKeyboardObservers(KeyboardInputAction action);
        void notifyMouseObservers(MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers);

        nk_context* mNkCtx = nullptr;
        Input::InputManager mInput;
        Misc::Point mMousePosition;
        bool mMouseDown = false;
        bool mClick = false;
        bool mRightMouseDown = false;
        bool mRightClick = false;
        Input::KeyboardModifiers mKbMods;
        FAGui::GuiManager* mGuiManager = nullptr;
        bool mPaused;
        std::map<KeyboardInputAction, Input::Hotkey> mHotkeys;
        std::vector<KeyboardInputObserverInterface*> mKeyboardObservers;
        std::vector<MouseInputObserverInterface*> mMouseObservers;
    };
}
