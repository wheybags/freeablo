#ifndef FA_RENDERER_H
#define FA_RENDERER_H

#include <stddef.h>

#include <boost/thread.hpp>

#include "../faworld/position.h"

namespace Level
{
    class DunFile;
}

namespace FARender
{
    class RenderState
    {
        public:

        boost::mutex mMutex;

        FAWorld::Position mPos;

        // some list of objects here later
    };

    class Renderer
    {
        public:
            Renderer();
            ~Renderer();
            bool setLevel(const Level::DunFile& dun, size_t level);

            RenderState* getFreeState(); // ooh ah up de ra
            void setCurrentState(RenderState* current);

        private:
            void renderLoop();
            
            boost::thread* mThread;            
            bool mLevelReady; 
            bool mDone;

            RenderState mStates[3];

            RenderState* mCurrent;
    };
}

#endif
