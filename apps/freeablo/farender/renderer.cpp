#include "renderer.h"

#include <assert.h>
#include <thread>

#include <input/inputmanager.h>
#include <audio/audio.h>
#include <misc/stringops.h>
#include <functional>
#include <iostream>

#include "../faworld/gamelevel.h"
#include "../engine/threadmanager.h"

namespace FARender
{
    FASpriteGroup defaultSprite;
    FASpriteGroup* getDefaultSprite()
    {
        return &defaultSprite;
    }

    Renderer* Renderer::mRenderer = NULL;

    Renderer* Renderer::get()
    {
        return mRenderer;
    }


    Renderer::Renderer(int32_t windowWidth, int32_t windowHeight, bool fullscreen)
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
            settings.fullscreen = fullscreen;

            Render::init(settings);

            mRocketContext = Render::initGui(std::bind(&Renderer::loadGuiTextureFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                                             std::bind(&Renderer::generateGuiTextureFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                                             std::bind(&Renderer::releaseGuiTextureFunc, this, std::placeholders::_1));
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

        FASpriteGroup* sprite = mSpriteManager.get(sourcePath);

        Render::RocketFATex* tex = new Render::RocketFATex();
        tex->animLength = sprite->getAnimLength();
        tex->spriteIndex = sprite->getCacheIndex();
        tex->index = celIndex;
        tex->needsImmortal = false;

        texture_dimensions.x = sprite->getWidth();
        texture_dimensions.y = sprite->getHeight();

        texture_handle = (Rocket::Core::TextureHandle) tex;
        return true;
    }

    bool Renderer::generateGuiTextureFunc(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
    {
        FASpriteGroup* sprite = mSpriteManager.getFromRaw(source, source_dimensions.x, source_dimensions.y);
        Render::RocketFATex* tex = new Render::RocketFATex();
        tex->spriteIndex = sprite->getCacheIndex();
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

    Tileset Renderer::getTileset(const FAWorld::GameLevel& gameLevel)
    {
        const Level::Level& level = gameLevel.mLevel;

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

    FASpriteGroup* Renderer::loadImage(const std::string& path)
    {
        return mSpriteManager.get(path);
    }

    FASpriteGroup* Renderer::loadServerImage(uint32_t index)
    {
        return mSpriteManager.getByServerSpriteIndex(index);
    }

    void Renderer::fillServerSprite(uint32_t index, const std::string& path)
    {
        mSpriteManager.fillServerSprite(index, path);
    }

    std::string Renderer::getPathForIndex(uint32_t index)
    {
        return mSpriteManager.getPathForIndex(index);
    }

    Render::Tile Renderer::getTileByScreenPos(size_t x, size_t y, const FAWorld::Position& screenPos)
    {
        return Render::getClickedTile(x, y, screenPos.current().first, screenPos.current().second, screenPos.next().first, screenPos.next().second, screenPos.mDist);
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
                    auto &object = state->mObjects[i];
                    FAWorld::Position & position = object.position;

                    size_t x = position.current().first;
                    size_t y = position.current().second;

                    mLevelObjects[x][y].valid = object.spriteGroup->isValid();
                    mLevelObjects[x][y].spriteCacheIndex = object.spriteGroup->getCacheIndex();
                    mLevelObjects[x][y].spriteFrame = object.frame;
                    mLevelObjects[x][y].x2 = position.next().first;
                    mLevelObjects[x][y].y2 = position.next().second;
                    mLevelObjects[x][y].dist = position.mDist;
                    mLevelObjects[x][y].hoverColor = object.hoverColor;
                }

                Render::drawLevel(state->level->mLevel, state->tileset.minTops->getCacheIndex(), state->tileset.minBottoms->getCacheIndex(),
                                  &mSpriteManager, mLevelObjects, state->mPos.current().first, state->mPos.current().second,
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
            Render::Sprite sprite = mSpriteManager.get(State->mCursorSpriteGroup->getCacheIndex())->operator [](State->mCursorFrame);
            Render::drawCursor(sprite, State->mCursorSpriteGroup->getWidth(), State->mCursorSpriteGroup->getHeight());
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
