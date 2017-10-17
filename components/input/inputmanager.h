#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <functional>

#include <boost/lockfree/spsc_queue.hpp>

#include "keys.h"


namespace Input
{
    struct KeyboardModifiers
    {
        bool ctrl = false;
        bool alt = false;
        bool shift = false;
    };

    class InputManager
    {
        public:
            InputManager(std::function<void(Key)> keyPress, std::function<void(Key)> keyRelease,
                std::function<void(uint32_t, uint32_t, Key, bool)> mouseClick,
                std::function<void(uint32_t, uint32_t, Key)> mouseRelease,
                std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> mouseMove,
                std::function<void(int32_t, int32_t)> mouseWheel,
                std::function<void(std::string)> textInput);

            bool processInput(bool paused); ///< \return true if user requested quit, false otherwise
            void poll();

            KeyboardModifiers getModifiers();

            static InputManager* get();

        private:

            bool mHasQuit = false;
            
            ///< Basically a copy of the subset of SDL_Event that we actually use
            struct Event
            {
                size_t type;

                union _vals
                {
                    int32_t key;
                    
                    struct _mouseButton
                    {
                        int32_t key;
                        int32_t x;
                        int32_t y;
                        int32_t numClicks;
                    } mouseButton;

                    struct _mouseMove
                    {
                        int32_t x;
                        int32_t y;
                        int32_t xrel;
                        int32_t yrel;
                    } mouseMove;

                    struct _mouseWheel
                    {
                        int32_t x;
                        int32_t y;
                    } mouseWheel;

                    struct _textInput
                    {
                        std::string* text;
                    } textInput;

                } vals;
            };

            boost::lockfree::spsc_queue<Event, boost::lockfree::capacity<500> > mQueue;
            std::function<void(Key)> mKeyPress;
            std::function<void(Key)> mKeyRelease;
            std::function<void(uint32_t, uint32_t, Key, bool)> mMouseClick;
            std::function<void(uint32_t, uint32_t, Key)> mMouseRelease;
            std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> mMouseMove;
            std::function<void(int32_t, int32_t)> mMouseWheel;
            std::function<void(std::string)> mTextInput;

            static InputManager* instance;

            KeyboardModifiers mModifiers;
    };
}

#endif
