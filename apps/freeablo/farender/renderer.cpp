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
    struct LoadGuiTextureStruct
    {
        Rocket::Core::TextureHandle* texture_handle;
        Rocket::Core::Vector2i* texture_dimensions;
        const Rocket::Core::String* source;
    };

    bool Renderer::loadGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
    {   
        LoadGuiTextureStruct* st = new LoadGuiTextureStruct();
        st->texture_handle = &texture_handle;
        st->texture_dimensions = &texture_dimensions;
        st->source = &source;

        mThreadCommunicationTmp = (void*) st;
        mRenderThreadState = guiLoadTexture;
        while(mRenderThreadState != running) {}

        delete st;

        return (bool) mThreadCommunicationTmp;;
    }

    struct GenerateGuiTextureStruct
    {
        Rocket::Core::TextureHandle* texture_handle;
        const Rocket::Core::byte* source;
        const Rocket::Core::Vector2i* source_dimensions;
    };

    bool Renderer::generateGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
    {
        GenerateGuiTextureStruct* st = new GenerateGuiTextureStruct();
        st->texture_handle = &texture_handle;
        st->source = source;
        st->source_dimensions = &source_dimensions;

        mThreadCommunicationTmp = (void*) st;
        mRenderThreadState = guiGenerateTexture;
        while(mRenderThreadState != running) {}
        
        delete st;

        return (bool) mThreadCommunicationTmp;
    }
    
    void Renderer::releaseGuiTextureFunc(Rocket::Core::TextureHandle texture_handle)
    {
        mThreadCommunicationTmp = (void*) &texture_handle;

        mRenderThreadState = guiReleaseTexture;
        while(mRenderThreadState != running) {}
    }                              

    Renderer::Renderer(int32_t windowWidth, int32_t windowHeight)
        :mRenderThreadState(stopped)
        ,mLevel(NULL)
        ,mDone(false)
        ,mCurrent(NULL)
        ,mRocketContext(NULL)
    {
        assert(!mRenderer); // singleton, only one instance

        // Render initialization.
        {
            Render::RenderSettings settings;
            settings.windowWidth = windowWidth;
            settings.windowHeight = windowHeight;

            Render::init(settings);
            mRocketContext = Render::initGui(boost::bind(&Renderer::loadGuiTextureFunc, this, _1, _2, _3), boost::bind(&Renderer::generateGuiTextureFunc, this, _1, _2, _3), boost::bind(&Renderer::releaseGuiTextureFunc, this, _1));

            mRenderer = this;
        }

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
        
    void Renderer::setLevel(const Level::Level* level)
    {
        if(level)
        {
            mThreadCommunicationTmp = (void*)level;
            mRenderThreadState = levelChange;
            while(mRenderThreadState != running){} // wait until the render thread is done loading the new level
        }
        else // no level, just start drawing gui
        {
            mLevel = NULL;
            mRenderThreadState = running;
        }
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

    Rocket::Core::Context* Renderer::getRocketContext()
    {
        return mRocketContext;
    }

    void Renderer::destroySprite(Render::SpriteGroup* s)
    {
        mThreadCommunicationTmp = (void*)s;
        mRenderThreadState = spriteDestroy;
        while(mRenderThreadState != running);
    }

    void Renderer::renderLoop()
    {
        Render::LevelObjects objects;

        while(!Input::InputManager::get()) {}

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

            else if(mRenderThreadState == guiLoadTexture)
            {
                LoadGuiTextureStruct* st = (LoadGuiTextureStruct*) mThreadCommunicationTmp;
                mThreadCommunicationTmp = (void*) Render::guiLoadImage(*(st->texture_handle), *(st->texture_dimensions), *(st->source));
                mRenderThreadState = running;
            }

            else if(mRenderThreadState == guiGenerateTexture)
            {
                GenerateGuiTextureStruct* st = (GenerateGuiTextureStruct*) mThreadCommunicationTmp;
                mThreadCommunicationTmp = (void*) Render::guiGenerateTexture(*(st->texture_handle), st->source, *(st->source_dimensions));
                mRenderThreadState = running;
            }

            else if(mRenderThreadState == guiReleaseTexture)
            {
                Render::guiReleaseTexture(*((Rocket::Core::TextureHandle*)mThreadCommunicationTmp));
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
                
                if(mLevel)
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
                }

                Render::drawGui(current->guiDrawBuffer);

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
