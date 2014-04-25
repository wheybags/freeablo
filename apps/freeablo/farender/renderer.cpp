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
        mRenderThreadState = stopped;
        mLevel = NULL;

        mCurrent = NULL;
        renderLoop();
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
        mRenderThreadState = levelChange;
        while(mRenderThreadState != running){} // wait until the render thread is done loading the new level
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
        mRenderThreadState = loadSprite;
        while(mRenderThreadState != running) {}

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
        mRenderThreadState = spriteDestroy;
        while(mRenderThreadState != running);
    }

    void Renderer::renderLoop()
    {
        Render::init();

        Render::LevelObjects objects;

        while(!mDone)
        {
            Input::InputManager::get()->poll();
             
            RenderState* current = mCurrent;

            if(mRenderThreadState == levelChange)
            {
                delete mLevel;
                Level::Level* level = (Level::Level*)mThreadCommunicationTmp;

                mLevel = Render::setLevel(*level);
                objects.resize(level->width(), level->height());

                mRenderThreadState = running;
            }

            else if(mRenderThreadState == loadSprite)
            {
                FASpriteGroup* tmp = new FASpriteGroup((CacheSpriteGroup*)NULL);
                *tmp = loadImageImp(*(std::string*)mThreadCommunicationTmp);
                mThreadCommunicationTmp = (void*)tmp;
                mRenderThreadState = running;
            }

            else if(mRenderThreadState == pause)
            {
                mRenderThreadState = stopped;
            }

            else if(mRenderThreadState == spriteDestroy)
            {
                Render::SpriteGroup* s = (Render::SpriteGroup*)mThreadCommunicationTmp;
                s->destroy();
                mRenderThreadState = running;
            }

            if(mRenderThreadState == running && current && current->mMutex.try_lock())
            {

                
                for(size_t x = 0; x < objects.width(); x++)
                {
                    for(size_t y = 0; y < objects.height(); y++)
                    {
                        objects[x][y].sprite = NULL;
                    }
                }

                for(size_t i = 0; i < current->mObjects.size(); i++)
                {
                    size_t x = current->mObjects[i].get<2>().current().first;
                    size_t y = current->mObjects[i].get<2>().current().second;

                    objects[x][y].sprite = (*current->mObjects[i].get<0>().get()).mSpriteGroup[current->mObjects[i].get<1>()];
                    objects[x][y].x2 = current->mObjects[i].get<2>().next().first;
                    objects[x][y].y2 = current->mObjects[i].get<2>().next().second;
                    objects[x][y].dist = current->mObjects[i].get<2>().mDist;
                }

                Render::drawLevel(mLevel, objects, current->mPos.current().first, current->mPos.current().second,
                    current->mPos.next().first, current->mPos.next().second, current->mPos.mDist);

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
