#include "renderer.h"

#include <assert.h>

#include <boost/thread.hpp>

#include <level/level.h>
#include <input/inputmanager.h>
#include <audio/audio.h>

#include "../engine/threadmanager.h"

namespace FARender
{
    Renderer* Renderer::mRenderer = NULL;

    Renderer* Renderer::get()
    {
        return mRenderer;
    }
                                  

    Renderer::Renderer(int32_t windowWidth, int32_t windowHeight)
        :mLevel(NULL)
        ,mDone(false)
        ,mCurrent(NULL)
        ,mRocketContext(NULL)
        ,mCache(1024)
    {
        assert(!mRenderer); // singleton, only one instance

        // Render initialization.
        {
            Render::RenderSettings settings;
            settings.windowWidth = windowWidth;
            settings.windowHeight = windowHeight;

            Render::init(settings);
            
            Engine::ThreadManager* threadManager = Engine::ThreadManager::get();
            mRocketContext = Render::initGui(boost::bind(&Engine::ThreadManager::loadGuiTextureFunc, threadManager, _1, _2, _3),
                                             boost::bind(&Engine::ThreadManager::generateGuiTextureFunc, threadManager, _1, _2, _3),
                                             boost::bind(&Engine::ThreadManager::releaseGuiTextureFunc, threadManager, _1));
            Audio::init();

            mRenderer = this;
        }
    }
    
    Renderer::~Renderer()
    {
        mRenderer = NULL;
        if(mLevel)
            delete mLevel;
        Render::quit();
    }

    void Renderer::stop()
    {
        mDone = true;
    }

    void Renderer::setLevel(Render::RenderLevel* renderLevel, const Level::Level* level)
    {
        if(mLevel)
            delete mLevel;
        mLevel = renderLevel;
        mLevelObjects.resize(level->width(), level->height());
    }

    void Renderer::setLevel(const Level::Level* level)
    {
        Engine::ThreadManager::get()->setLevel(level);
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
        return mCache.get(path);
    }

    std::pair<size_t, size_t> Renderer::getClickedTile(size_t x, size_t y)
    {
        return Render::getClickedTile(mLevel, x, y);
    }

    Rocket::Core::Context* Renderer::getRocketContext()
    {
        return mRocketContext;
    }

    bool Renderer::renderFrame()
    {
        if(mDone)
            return false;

        RenderState* current = mCurrent;

        if(current && current->mMutex.try_lock())
        {
            
            if(mLevel)
            {
                for(size_t x = 0; x < mLevelObjects.width(); x++)
                {
                    for(size_t y = 0; y < mLevelObjects.height(); y++)
                    {
                        mLevelObjects[x][y].valid = false;
                    }
                }

                for(size_t i = 0; i < current->mObjects.size(); i++)
                {
                    size_t x = current->mObjects[i].get<2>().current().first;
                    size_t y = current->mObjects[i].get<2>().current().second;

                    mLevelObjects[x][y].valid = true;
                    mLevelObjects[x][y].spriteCacheIndex = current->mObjects[i].get<0>().spriteCacheIndex;
                    mLevelObjects[x][y].spriteFrame = current->mObjects[i].get<1>();
                    mLevelObjects[x][y].x2 = current->mObjects[i].get<2>().next().first;
                    mLevelObjects[x][y].y2 = current->mObjects[i].get<2>().next().second;
                    mLevelObjects[x][y].dist = current->mObjects[i].get<2>().mDist;
                }

                Render::drawLevel(mLevel, &mCache, mLevelObjects, current->mPos.current().first, current->mPos.current().second,
                    current->mPos.next().first, current->mPos.next().second, current->mPos.mDist);
            }

            Render::drawGui(current->guiDrawBuffer);

            current->mMutex.unlock();
        }
        
        Render::draw();

        return true;
    }

    void Renderer::cleanup()
    {
        mCache.clear();
    }
}
