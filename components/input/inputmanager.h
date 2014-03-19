#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#include <boost/function.hpp>
#include <boost/thread.hpp>

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

            void poll();

        private:
            boost::function<void(Key)> mKeyPress;
            boost::function<void(Key)> mKeyRelease;
            boost::function<void(uint32_t, uint32_t, Key)> mMouseClick;
            boost::function<void(uint32_t, uint32_t, Key)> mMouseRelease;
            boost::function<void(uint32_t, uint32_t)> mMouseMove;
    };
}

#endif
