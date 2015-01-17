#include "renderer.h"

#include <assert.h>

#include <boost/thread.hpp>

#include <level/level.h>
#include <input/inputmanager.h>
#include <audio/audio.h>
#include <misc/stringops.h>

#include "../engine/threadmanager.h"

namespace FARender
{
    Renderer* Renderer::mRenderer = NULL;

    Renderer* Renderer::get()
    {
        return mRenderer;
    }
                                  

    Renderer::Renderer(int32_t windowWidth, int32_t windowHeight)
        :mDone(false)
        ,mCurrent(NULL)
        ,mRocketContext(NULL)
        ,mSpriteManager(1024)
    {
        assert(!mRenderer); // singleton, only one instance

        // Render initialization.
        {
            Render::RenderSettings settings;
            settings.windowWidth = windowWidth;
            settings.windowHeight = windowHeight;

            Render::init(settings);
            
            mRocketContext = Render::initGui(boost::bind(&Renderer::loadGuiTextureFunc, this, _1, _2, _3),
                                             boost::bind(&Renderer::generateGuiTextureFunc, this, _1, _2, _3),
                                             boost::bind(&Renderer::releaseGuiTextureFunc, this, _1));
            Audio::init();

            mRenderer = this;
        }
    }

    bool Renderer::loadGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
    {
        std::vector<std::string> components = Misc::StringUtils::split(std::string(source.CString()), '&');

        size_t celIndex = 0;
        std::string sourcePath = components[0];

        for(size_t i = 1; i < components.size(); i++)
        {
            std::vector<std::string> pair = Misc::StringUtils::split(components[i], '=');

            if(pair.size() != 2)
            {
                std::cerr << "Invalid image filename param " << components[i] << std::endl;
                continue;
            }

            if(pair[0] == "frame")
            {
                std::istringstream ss2(pair[1]);
                ss2 >> celIndex;
            }
            else if(pair[0] == "trans")
            {
                // forward trans params on to be dealt with later in SpriteCache
                sourcePath += std::string("&") + components[i];
            }
        }

        FASpriteGroup sprite = mSpriteManager.get(sourcePath);

        Render::RocketFATex* tex = new Render::RocketFATex();
        tex->spriteIndex = sprite.spriteCacheIndex;
        tex->index = celIndex;
        tex->needsImmortal = false;

        texture_dimensions.x = sprite.width;
        texture_dimensions.y = sprite.height;

        texture_handle = (Rocket::Core::TextureHandle) tex;
        return true;
    }

    bool Renderer::generateGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
    {
        FASpriteGroup sprite = mSpriteManager.getFromRaw(source, source_dimensions.x, source_dimensions.y);
        Render::RocketFATex* tex = new Render::RocketFATex();
        tex->spriteIndex = sprite.spriteCacheIndex;
        tex->index = 0;
        tex->needsImmortal = true;

        texture_handle = (Rocket::Core::TextureHandle) tex;
        return true;
    }

    void Renderer::releaseGuiTextureFunc(Rocket::Core::TextureHandle texture_handle)
    {
        Render::RocketFATex* tex = (Render::RocketFATex*)texture_handle;

        if(tex->needsImmortal)
            mSpriteManager.setImmortal(tex->spriteIndex, false);

        delete tex;
    }

    Renderer::~Renderer()
    {
        mRenderer = NULL;
        Render::quit();
    }

    void Renderer::stop()
    {
        mDone = true;
    }

    Tileset Renderer::getTileset(const Level::Level& level)
    {
        Tileset tileset;
        tileset.minTops = mSpriteManager.getTileset(level.getTileSetPath(), level.getMinPath(), true);
        tileset.minBottoms = mSpriteManager.getTileset(level.getTileSetPath(), level.getMinPath(), false);
        return tileset;
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
        return mSpriteManager.get(path);
    }

    std::pair<size_t, size_t> Renderer::getClickedTile(size_t x, size_t y, const Level::Level& level, const FAWorld::Position& screenPos)
    {
        return Render::getClickedTile(level, x, y, screenPos.current().first, screenPos.current().second, screenPos.next().first, screenPos.next().second, screenPos.mDist);
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
            
            if(current->level)
            {
                if(mLevelObjects.width() != current->level->width() || mLevelObjects.height() != current->level->height())
                    mLevelObjects.resize(current->level->width(), current->level->height());

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

                Render::drawLevel(*current->level, current->tileset.minTops.spriteCacheIndex, current->tileset.minBottoms.spriteCacheIndex, &mSpriteManager, mLevelObjects, current->mPos.current().first, current->mPos.current().second,
                    current->mPos.next().first, current->mPos.next().second, current->mPos.mDist);
            }

            Render::drawGui(current->guiDrawBuffer, &mSpriteManager);

            current->mMutex.unlock();
        }
        
        Render::draw();

        return true;
    }

    void Renderer::cleanup()
    {
        mSpriteManager.clear();
    }
}
