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
            mRenderer = this;
        }
    }

    bool Renderer::loadGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
    {
        std::vector<std::string> components = Misc::StringUtils::split(std::string(source.CString()), '&');

        size_t celIndex = 0;

        if(components.size() == 0)
            return false;
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
            else
            {
                // forward other params on to be dealt with later in SpriteCache
                sourcePath += std::string("&") + components[i];
            }
        }

        FASpriteGroup sprite = mSpriteManager.get(sourcePath);

        Render::RocketFATex* tex = new Render::RocketFATex();
        tex->animLength = sprite.animLength;
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
        Render::quitGui();
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
        for(size_t i = 0; i < 15; i++)
        {
            if(mStates[i].ready)
            {
                mStates[i].ready = false;
                return &mStates[i];
            }
        }

        return NULL;
    }

    void Renderer::setCurrentState(RenderState* current)
    {
        Engine::ThreadManager::get()->sendRenderState(current);
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

    void Renderer::waitUntilDone()
    {
        std::unique_lock<std::mutex> lk(mDoneMutex);
        if(!mAlreadyExited)
            mDoneCV.wait(lk);
    }

    bool Renderer::renderFrame(RenderState* state)
    {
        if(mDone)
        {
            {
                std::unique_lock<std::mutex> lk(mDoneMutex);
                mAlreadyExited = true;
            }
            mDoneCV.notify_one();
            return false;
        }

        if(state)
        {
            
            if(state->level)
            {
                if(mLevelObjects.width() != state->level->width() || mLevelObjects.height() != state->level->height())
                    mLevelObjects.resize(state->level->width(), state->level->height());

                for(size_t x = 0; x < mLevelObjects.width(); x++)
                {
                    for(size_t y = 0; y < mLevelObjects.height(); y++)
                    {
                        mLevelObjects[x][y].valid = false;
                    }
                }

                for(size_t i = 0; i < state->mObjects.size(); i++)
                {
                    size_t x = state->mObjects[i].get<2>().current().first;
                    size_t y = state->mObjects[i].get<2>().current().second;

                    mLevelObjects[x][y].valid = true;
                    mLevelObjects[x][y].spriteCacheIndex = state->mObjects[i].get<0>().spriteCacheIndex;
                    mLevelObjects[x][y].spriteFrame = state->mObjects[i].get<1>();
                    mLevelObjects[x][y].x2 = state->mObjects[i].get<2>().next().first;
                    mLevelObjects[x][y].y2 = state->mObjects[i].get<2>().next().second;
                    mLevelObjects[x][y].dist = state->mObjects[i].get<2>().mDist;
                }

                Render::drawLevel(*state->level, state->tileset.minTops.spriteCacheIndex, state->tileset.minBottoms.spriteCacheIndex, &mSpriteManager, mLevelObjects, state->mPos.current().first, state->mPos.current().second,
                    state->mPos.next().first, state->mPos.next().second, state->mPos.mDist);
            }

            Render::drawGui(state->guiDrawBuffer, &mSpriteManager);
            Renderer::setCursor(state);
        }
        
        Render::draw();

        return true;
    }
    void Renderer::setCursor(RenderState * State)
    {

        if(!State->mCursorEmpty)
        {
            Render::Sprite sprite = mSpriteManager.get(State->mCursorSpriteGroup.spriteCacheIndex)->operator [](State->mCursorFrame);
            Render::drawCursor(sprite);
        }
        else
        {
            Render::drawCursor(NULL);
        }
        return;

    }

    void Renderer::cleanup()
    {
        mSpriteManager.clear();
    }
}
