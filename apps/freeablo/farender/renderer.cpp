#include "renderer.h"

#include <boost/thread.hpp>

#include <level/dunfile.h>
#include <level/tilfile.h>
#include <level/minfile.h>

#include <render/render.h>

namespace FARender
{
    Renderer::Renderer()
    {
        mDone = false;
        mLevelReady = false;

        mCurrent = NULL;

        Render::init();

        mThread = new boost::thread(boost::bind(&Renderer::renderLoop, this));
    }
    
    Renderer::~Renderer()
    {
        mDone = true;
        mThread->join();
        delete mThread;
    }
        
    void Renderer::setLevel(const Level::DunFile& dun, size_t level)
    {
        Level::MinFile min("levels/l1data/l1.min");
        Level::TilFile til("levels/l1data/l1.til");

        Render::setLevel("levels/l1data/l1.cel", dun, til, min);
        
        mLevelReady = true;
    }
    
    RenderState* Renderer::getFreeState()
    {
        while(true)
        {
            for(size_t i = 0; i < 3; i++)
            {
                if(&mStates[i] != mCurrent && mStates[i].mMutex.try_lock())
                    return &mStates[i];
            }
        }
            
        return NULL;
    }

    void Renderer::setCurrentState(RenderState* current)
    {
        current->mMutex.unlock();
        mCurrent = current;
    }

    void Renderer::renderLoop()
    {
        while(!mDone)
        {
            RenderState* current = mCurrent;

            if(mLevelReady && current && current->mMutex.try_lock())
            {
                Render::drawLevel(current->mX1, current->mY1, current->mX2, current->mY2, current->mDist);

                current->mMutex.unlock();
            }

            Render::draw();
        }
    }
}
