#include "renderer.h"

#include <assert.h>

#include <boost/thread.hpp>

#include <level/level.h>

#include <input/inputmanager.h>

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
        mRenderReady = 99;
        mLevel = NULL;

        mCurrent = NULL;
        renderLoop();
        //mThread = new boost::thread(boost::bind(&Renderer::renderLoop, this));
    }
    
    Renderer::~Renderer()
    {
        mRenderer = NULL;
        delete mLevel;
        Render::quit();
    }

    void Renderer::stop()
    {
        mDone = true;
    }
        
    void Renderer::setLevel(const Level::Level& level)
    {
        mThreadCommunicationTmp = (void*)&level;
        mRenderReady = 1;
        while(mRenderReady != 0){} // wait until the render thread is done loading the new level
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
        mThreadCommunicationTmp = (void*)&path;
        mRenderReady = 2;
        while(mRenderReady != 0) {}

        FASpriteGroup tmp = *(FASpriteGroup*)mThreadCommunicationTmp;
        delete (FASpriteGroup*)mThreadCommunicationTmp;
        return tmp;
    }

    FASpriteGroup Renderer::loadImageImp(const std::string& path)
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

    void Renderer::destroySprite(Render::SpriteGroup* s)
    {
        mThreadCommunicationTmp = (void*)s;
        mRenderReady = 5;
        while(mRenderReady != 0);
    }

    void Renderer::renderLoop()
    {
        Render::init();

        while(!mDone)
        {
            Input::InputManager::get()->poll();
             
            RenderState* current = mCurrent;

            if(mRenderReady == 1)
            {
                delete mLevel;
                mLevel = Render::setLevel(*(Level::Level*)mThreadCommunicationTmp);
                mRenderReady = 0;
            }

            else if(mRenderReady == 2)
            {
                FASpriteGroup* tmp = new FASpriteGroup((CacheSpriteGroup*)NULL);
                *tmp = loadImageImp(*(std::string*)mThreadCommunicationTmp);
                mThreadCommunicationTmp = (void*)tmp;
                mRenderReady = 0;
            }

            else if(mRenderReady == 3)
                mRenderReady++;

            else if(mRenderReady == 5)
            {
                Render::SpriteGroup* s = (Render::SpriteGroup*)mThreadCommunicationTmp;
                s->destroy();
                mRenderReady = 0;
            }

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
        
        // destroy all remaining sprites here, otherwise they would be destoyed in the game thread, which would not work 
        for(std::map<std::string, boost::weak_ptr<CacheSpriteGroup> >::iterator it = mSpriteCache.begin(); it != mSpriteCache.end(); ++it)
        {
            it->second.lock().get()->destroy();
        }
    }
}
