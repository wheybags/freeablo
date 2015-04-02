#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#include <boost/function.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <misc/disablewarn.h>
#include <Rocket/Core.h>
#include <misc/enablewarn.h>

#include "keys.h"

namespace Input
{
    enum Modifier
    {
        FAMOD_CTRL = 1,
        FAMOD_ALT = 2,
        FAMOD_SHIFT = 4
    };

    class InputManager
    {
        public:
            InputManager(boost::function<void(Key)> keyPress, boost::function<void(Key)> keyRelease,
                boost::function<void(uint32_t, uint32_t, Key)> mouseClick,
                boost::function<void(uint32_t, uint32_t, Key)> mouseRelease,
                boost::function<void(uint32_t, uint32_t)> mouseMove,
                Rocket::Core::Context* context);

            void processInput(bool paused);
            void poll();

            uint32_t getModifiers();

            static InputManager* get();

        private:
            void rocketBaseClicked(); ///< called by libRocket when it receives a click that doesn't hit any gui elements
            bool mBaseWasClicked;

            friend void baseClickedHelper();

            
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

            boost::lockfree::spsc_queue<Event, boost::lockfree::capacity<500> > mQueue;
            boost::function<void(Key)> mKeyPress;
            boost::function<void(Key)> mKeyRelease;
            boost::function<void(uint32_t, uint32_t, Key)> mMouseClick;
            boost::function<void(uint32_t, uint32_t, Key)> mMouseRelease;
            boost::function<void(uint32_t, uint32_t)> mMouseMove;

            Rocket::Core::Context* mContext;

            static InputManager* instance;

            uint32_t mModifiers;
    };
}

#endif
