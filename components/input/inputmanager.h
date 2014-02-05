#ifndef INPUT_H
#define INPUT_H

#include <boost/function.hpp>
#include <boost/thread.hpp>

#include "keys.h"

namespace Input
{
    class InputManager
    {
        public:
            InputManager(boost::function<void(Key)> keyPress, boost::function<void(Key)> keyRelease);
            ~InputManager();

            void inputLoop();

        private:
            boost::function<void(Key)> mKeyPress;
            boost::function<void(Key)> mKeyRelease;

            boost::thread mThread;
            bool mDone;

    };
}

#endif
