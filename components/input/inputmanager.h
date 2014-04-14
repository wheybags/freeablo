#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#include <boost/function.hpp>
#include <boost/lockfree/queue.hpp>

#include "keys.h"

namespace Input
{
    class InputManager
    {
        public:
            InputManager(boost::function<void(Key)> keyPress, boost::function<void(Key)> keyRelease,
                boost::function<void(uint32_t, uint32_t, Key)> mouseClick,
                boost::function<void(uint32_t, uint32_t, Key)> mouseRelease,
                boost::function<void(uint32_t, uint32_t)> mouseMove);

            void processInput();
            void poll();

            static InputManager* get();

        private:
            
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
                    } mouseButton;

                    struct _mouseMove
                    {
                        int32_t x;
                        int32_t y;
                    } mouseMove;

                } vals;
            };

            boost::lockfree::queue<Event> mQueue;
            boost::function<void(Key)> mKeyPress;
            boost::function<void(Key)> mKeyRelease;
            boost::function<void(uint32_t, uint32_t, Key)> mMouseClick;
            boost::function<void(uint32_t, uint32_t, Key)> mMouseRelease;
            boost::function<void(uint32_t, uint32_t)> mMouseMove;

            static InputManager* instance;
    };
}

#endif
