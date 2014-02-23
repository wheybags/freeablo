#include "renderer.h"

#include <assert.h>

#include <boost/thread.hpp>

#include <level/dun.h>
#include <level/tileset.h>
#include <level/min.h>

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
        mLevelReady = false;

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
    }
        
    bool Renderer::setLevel(const Level::Dun& dun, size_t level)
    {
        switch(level)
        {
            case 0:
            {
                Render::setLevel("levels/towndata/town.cel", dun, 
                    Level::TileSet("levels/towndata/town.til"),
                    Level::Min("levels/towndata/town.min"));
                break;
            }
            case 1:
            {
                Render::setLevel("levels/l1data/l1.cel", dun, 
                    Level::TileSet("levels/l1data/l1.til"),
                    Level::Min("levels/l1data/l1.min"));
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
        
        mLevelReady = true;
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

            if(mLevelReady && current && current->mMutex.try_lock())
            {
                Render::drawLevel(current->mPos.current().first, current->mPos.current().second, 
                    current->mPos.next().first, current->mPos.next().second, current->mPos.mDist);

                for(size_t i = 0; i < current->mObjects.size(); i++)
                {
                    Render::drawAt((*current->mObjects[i].get<0>().get()).mSpriteGroup[current->mObjects[i].get<1>()], current->mObjects[i].get<2>().current().first, current->mObjects[i].get<2>().current().second,
                        current->mObjects[i].get<2>().next().first, current->mObjects[i].get<2>().next().second, current->mObjects[i].get<2>().mDist);
                }

                current->mMutex.unlock();
            }

            Render::draw();
        }
    }
}
