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
        
    bool Renderer::setLevel(const Level::Level& map, size_t level)
    {
        switch(level)
        {
            case 0:
            {
                mLevel = Render::setLevel(map, "levels/towndata/town.cel");
                break;
            }
            case 1:
            {
                mLevel = Render::setLevel(map, "levels/l1data/l1.cel");
                break;
            }
            case 2:
            case 3:
            case 4:
            {
                std::cerr << "level " << level << " not yet implemented" << std::endl;
                return false;
            }
        }
        
        return true;
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

    void Renderer::renderLoop()
    {
        while(!mDone)
        {
            RenderState* current = mCurrent;

            if(mLevel && current && current->mMutex.try_lock())
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
