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

    void nk_fa_font_stash_begin(nk_font_atlas& atlas)
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);
    }

    nk_handle nk_fa_font_stash_end(SpriteManager& spriteManager, nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
    {
        const void *image; int w, h;
        image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

        FASpriteGroup* sprite = spriteManager.getFromRaw((uint8_t*)image, w, h);
        spriteManager.setImmortal(sprite->getCacheIndex(), true);

        nk_handle handle = sprite->getNkImage().handle;
        nk_font_atlas_end(&atlas, handle, &nullTex);

        if (atlas.default_font)
            nk_style_set_font(ctx, &atlas.default_font->handle);

        return handle;
    }
                                  
    Renderer::Renderer(int32_t windowWidth, int32_t windowHeight, bool fullscreen)
        :mDone(false)
        ,mSpriteManager(1024)
        ,mWidthHeightTmp(0)
    {
        assert(!mRenderer); // singleton, only one instance

        // Render initialization.
        {
            Render::RenderSettings settings;
            settings.windowWidth = windowWidth;
            settings.windowHeight = windowHeight;
            settings.fullscreen = fullscreen;

            nk_init_default(&mNuklearContext, nullptr);
            mNuklearContext.clip.copy = nullptr;// nk_sdl_clipbard_copy;
            mNuklearContext.clip.paste = nullptr;// nk_sdl_clipbard_paste;
            mNuklearContext.clip.userdata = nk_handle_ptr(0);

            Render::init(settings, mNuklearGraphicsData, &mNuklearContext);

            // Load Fonts: if none of these are loaded a default font will be used
            // Load Cursor: if you uncomment cursor loading please hide the cursor
            {
                nk_fa_font_stash_begin(mNuklearGraphicsData.atlas);
                //struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
                //struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
                //struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
                //struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
                //struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
                //struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
                mNuklearGraphicsData.dev.font_tex = nk_fa_font_stash_end(mSpriteManager, &mNuklearContext, mNuklearGraphicsData.atlas, mNuklearGraphicsData.dev.null);
                //nk_style_load_all_cursors(ctx, atlas->cursors);
                //nk_style_set_font(ctx, &roboto->handle);
            }



            mStates = (RenderState*)malloc(sizeof(RenderState) * mNumRenderStates);

            for (size_t i = 0; i < mNumRenderStates; ++i)
                new (mStates + i) RenderState(mNuklearGraphicsData);
            
            mRenderer = this;
        }
    }

    Renderer::~Renderer()
    {
        mRenderer = NULL;

        for (size_t i = 0; i < mNumRenderStates; ++i)
            mStates[i].~RenderState();

        free(mStates);
        destroyNuklearGraphicsContext(mNuklearGraphicsData);
        nk_free(&mNuklearContext);

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
        for(size_t i = 0; i < mNumRenderStates; i++)
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

    Render::Tile Renderer::getClickedTile(size_t x, size_t y, const FAWorld::Position& screenPos)
    {
        return Render::getClickedTile(x, y, screenPos.current().first, screenPos.current().second, screenPos.next().first, screenPos.next().second, screenPos.mDist);
    }

    void Renderer::waitUntilDone()
    {
        std::unique_lock<std::mutex> lk(mDoneMutex);
        if(!mAlreadyExited)
            mDoneCV.wait(lk);
    }

    union I32sAs64
    {
        int32_t int32s[2];
        int64_t int64;
    };

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
                        if (mLevelObjects[x][y].size() > 0) {
                            mLevelObjects[x][y].clear();
                        }
                    }
                }

                for(size_t i = 0; i < state->mObjects.size(); i++)
                {
                    FAWorld::Position & position = std::get<2>(state->mObjects[i]);

                    size_t x = position.current().first;
                    size_t y = position.current().second;
                    Render::LevelObject levelObject = {
                        std::get<0>(state->mObjects[i])->isValid(),
                        std::get<0>(state->mObjects[i])->getCacheIndex(),
                        std::get<1>(state->mObjects[i]),
                        position.next().first,
                        position.next().second,
                        position.mDist
                    };

                    mLevelObjects[x][y].push_back(levelObject);
                }

                Render::drawLevel(state->level->mLevel, state->tileset.minTops->getCacheIndex(), state->tileset.minBottoms->getCacheIndex(), &mSpriteManager, mLevelObjects, state->mPos.current().first, state->mPos.current().second,
                    state->mPos.next().first, state->mPos.next().second, state->mPos.mDist);

                Render::drawGui(state->nuklearData, &mSpriteManager);
            }
            
            Renderer::setCursor(state);
        }

        Render::draw();

        I32sAs64 tmp;
        tmp.int32s[0] = Render::WIDTH;
        tmp.int32s[1] = Render::HEIGHT;

        mWidthHeightTmp = tmp.int64;

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

    void Renderer::getWindowDimensions(int32_t& w, int32_t& h)
    {
        I32sAs64 tmp;
        tmp.int64 = mWidthHeightTmp;

        w = tmp.int32s[0];
        h = tmp.int32s[1];
    }
}
