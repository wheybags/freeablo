#include "renderer.h"

#include <assert.h>

#include <boost/thread.hpp>

#include <level/level.h>

namespace FARender
{
    Renderer* Renderer::mRenderer = NULL;

    Renderer* Renderer::get()
    {
        return mRenderer;
    }

    Renderer::Renderer()
    {
        assert(!mRenderer); // singleton, only one instance
        mRenderer = this;

        mDone = false;
        mRenderReady = 1;
        mLevel = NULL;

        mCurrent = NULL;

        Render::init();

        mThread = new boost::thread(boost::bind(&Renderer::renderLoop, this));
    }
    
    Renderer::~Renderer()
    {
        mRenderer = NULL;
        mDone = true;
        mThread->join();
        delete mThread;
        delete mLevel;
    }
        
    void Renderer::setLevel(const Level::Level& level)
    {
        mRenderReady = 1;
        while(mRenderReady != 2){} // wait until the render thread is definitely done

        delete mLevel;
        mLevel = Render::setLevel(level);

        mRenderReady = 0;
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

    FASpriteGroup Renderer::loadImage(const std::string& path)
    {
        bool contains = mSpriteCache.find(path) != mSpriteCache.end();

        if(contains)
        {
            FASpriteGroup cached = mSpriteCache[path].lock();
            if(cached)
                return cached;
        }
        
        FASpriteGroup newSprite(new CacheSpriteGroup(path));
        mSpriteCache[path] = boost::weak_ptr<CacheSpriteGroup>(newSprite);

        return newSprite;
    }
    
    std::pair<size_t, size_t> Renderer::getClickedTile(size_t x, size_t y)
    {
        return Render::getClickedTile(mLevel, x, y);
    }

    void Renderer::renderLoop()
    {
        while(!mDone)
        {
            RenderState* current = mCurrent;

            if(mRenderReady == 1)
                mRenderReady = 2;

            if(mRenderReady == 0 && current && current->mMutex.try_lock())
            {
                Render::drawLevel(mLevel, current->mPos.current().first, current->mPos.current().second, 
                    current->mPos.next().first, current->mPos.next().second, current->mPos.mDist);

                for(size_t i = 0; i < current->mObjects.size(); i++)
                {
                    Render::drawAt(mLevel, (*current->mObjects[i].get<0>().get()).mSpriteGroup[current->mObjects[i].get<1>()], current->mObjects[i].get<2>().current().first, current->mObjects[i].get<2>().current().second,
                        current->mObjects[i].get<2>().next().first, current->mObjects[i].get<2>().next().second, current->mObjects[i].get<2>().mDist);
                }

                current->mMutex.unlock();
            }

            Render::draw();
        }
    }
}
