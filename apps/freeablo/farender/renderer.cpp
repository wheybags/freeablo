#include "renderer.h"

#include <thread>

#include <audio/audio.h>
#include <functional>
#include <input/inputmanager.h>
#include <iostream>
#include <misc/assert.h>
#include <misc/stringops.h>

#include "../engine/threadmanager.h"
#include "../fagui/guimanager.h"
#include "../faworld/gamelevel.h"
#include "cel/celdecoder.h"
#include "fontinfo.h"
#include <boost/format.hpp>
#include <boost/range/irange.hpp>
#include <numeric>

namespace FARender
{
    FASpriteGroup* getDefaultSprite()
    {
        static FASpriteGroup defaultSprite;
        return &defaultSprite;
    }

    Renderer* Renderer::mRenderer = NULL;

    std::unique_ptr<CelFontInfo> Renderer::generateCelFont(const std::string& texturePath, const DiabloExe::FontData& fontData, int spacing)
    {
        std::unique_ptr<CelFontInfo> ret(new CelFontInfo());
        auto mergedTex = mSpriteManager.get(texturePath + "&convertToSingleTexture");
        ret->initByFontData(fontData, mergedTex->getWidth(), spacing);
        ret->nkFont.userdata.ptr = ret.get();
        ret->nkFont.height = mergedTex->getHeight();
        ret->nkFont.width = &CelFontInfo::getWidth;
        mSpriteManager.get(texturePath);
        ret->nkFont.query = &CelFontInfo::queryGlyph;
        ret->nkFont.texture = mergedTex->getNkImage().handle;
        return ret;
    }

    std::unique_ptr<PcxFontInfo> Renderer::generateFont(const std::string& pcxPath, const std::string& binPath)
    {
        std::unique_ptr<PcxFontInfo> ret(new PcxFontInfo());
        auto tex = mSpriteManager.get(pcxPath + "&trans=0,255,0");
        ret->initWidths(binPath, tex->getWidth());
        ret->nkFont.userdata.ptr = ret.get();
        ret->nkFont.height = tex->getHeight() / PcxFontInfo::charCount;
        ret->nkFont.width = &PcxFontInfo::getWidth;
        mSpriteManager.get(pcxPath);
        ret->nkFont.query = &PcxFontInfo::queryGlyph;
        ret->nkFont.texture = tex->getNkImage().handle;
        return ret;
    }

    Renderer* Renderer::get() { return mRenderer; }

    void nk_fa_font_stash_begin(nk_font_atlas& atlas)
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);
    }

    nk_handle nk_fa_font_stash_end(SpriteManager& spriteManager, nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
    {
        const void* image;
        int w, h;
        image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

        FASpriteGroup* sprite = spriteManager.getFromRaw((uint8_t*)image, w, h);
        spriteManager.setImmortal(sprite->getCacheIndex(), true);

        nk_handle handle = sprite->getNkImage().handle;
        nk_font_atlas_end(&atlas, handle, &nullTex);

        if (atlas.default_font)
            nk_style_set_font(ctx, &atlas.default_font->handle);

        return handle;
    }

    Renderer::Renderer(int32_t windowWidth, int32_t windowHeight, bool fullscreen) : mDone(false), mSpriteManager(1024), mWidthHeightTmp(0)
    {
        release_assert(!mRenderer); // singleton, only one instance

        // Render initialization.
        {
            Render::RenderSettings settings;
            settings.windowWidth = windowWidth;
            settings.windowHeight = windowHeight;
            settings.fullscreen = fullscreen;

            nk_init_default(&mNuklearContext, nullptr);
            mNuklearContext.clip.copy = nullptr;  // nk_sdl_clipbard_copy;
            mNuklearContext.clip.paste = nullptr; // nk_sdl_clipbard_paste;
            mNuklearContext.clip.userdata = nk_handle_ptr(0);

            Render::init("Freeablo", settings, mNuklearGraphicsData, &mNuklearContext);

            // Load Fonts: if none of these are loaded a default font will be used
            // Load Cursor: if you uncomment cursor loading please hide the cursor
            {
                nk_fa_font_stash_begin(mNuklearGraphicsData.atlas);
                // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
                // struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
                // struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
                // struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
                // struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
                // struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
                mNuklearGraphicsData.dev.font_tex =
                    nk_fa_font_stash_end(mSpriteManager, &mNuklearContext, mNuklearGraphicsData.atlas, mNuklearGraphicsData.dev.null);
                // nk_style_load_all_cursors(ctx, atlas->cursors);
                // nk_style_set_font(ctx, &roboto->handle);
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

    void Renderer::stop() { mDone = true; }

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
        for (size_t i = 0; i < mNumRenderStates; i++)
        {
            if (mStates[i].ready)
            {
                mStates[i].ready = false;
                return &mStates[i];
            }
        }

        return NULL;
    }

    void Renderer::setCurrentState(RenderState* current) { Engine::ThreadManager::get()->sendRenderState(current); }

    FASpriteGroup* Renderer::loadImage(const std::string& path) { return mSpriteManager.get(path); }

    FASpriteGroup* Renderer::loadServerImage(uint32_t index) { return mSpriteManager.getByServerSpriteIndex(index); }

    void Renderer::fillServerSprite(uint32_t index, const std::string& path) { mSpriteManager.fillServerSprite(index, path); }

    std::string Renderer::getPathForIndex(uint32_t index) { return mSpriteManager.getPathForIndex(index); }

    Render::Tile Renderer::getTileByScreenPos(size_t x, size_t y, const FAWorld::Position& screenPos)
    {
        return Render::getTileByScreenPos(
            x, y, screenPos.current().first, screenPos.current().second, screenPos.next().first, screenPos.next().second, screenPos.getDist());
    }

    void Renderer::waitUntilDone()
    {
        std::unique_lock<std::mutex> lk(mDoneMutex);
        if (!mAlreadyExited)
            mDoneCV.wait(lk);
    }

    union I32sAs64
    {
        int32_t int32s[2];
        int64_t int64;
    };

    static void fill(const FAWorld::GameLevel& level, const std::vector<ObjectToRender> src, Render::LevelObjects& dst)
    {
        if (dst.width() != level.width() || dst.height() != level.height())
            dst.resize(level.width(), level.height());

        for (int32_t x = 0; x < dst.width(); x++)
        {
            for (int32_t y = 0; y < dst.height(); y++)
            {
                dst[x][y].clear();
            }
        }

        for (size_t i = 0; i < src.size(); i++)
        {
            auto& object = src[i];
            auto& position = object.position;

            Render::LevelObject obj;
            obj.spriteCacheIndex = object.spriteGroup->getCacheIndex();
            obj.spriteFrame = object.frame;
            obj.x2 = position.next().first;
            obj.y2 = position.next().second;
            obj.dist = position.getDist();
            obj.hoverColor = object.hoverColor;
            obj.valid = true;

            size_t x = position.current().first;
            size_t y = position.current().second;
            dst[x][y].push_back(std::move(obj));
        }
    }

    bool Renderer::renderFrame(RenderState* state, const std::vector<uint32_t>& spritesToPreload)
    {
        if (mDone)
        {
            {
                std::unique_lock<std::mutex> lk(mDoneMutex);
                mAlreadyExited = true;
            }
            mDoneCV.notify_one();
            return false;
        }

        Render::clear(0, 0, 0);

        // force preloading of sprites by drawing them offscreen
        for (auto id : spritesToPreload)
        {
            Render::SpriteGroup* sprite = mSpriteManager.get(id);
            for (size_t i = 0; i < sprite->size(); i++)
                Render::drawSprite(sprite->operator[](i), Render::WIDTH + 10, 0);
        }

        if (state)
        {
            if (state->level)
            {
                if (mLevelObjects.width() != state->level->width() || mLevelObjects.height() != state->level->height())
                    mLevelObjects.resize(state->level->width(), state->level->height());

                for (int32_t x = 0; x < mLevelObjects.width(); x++)
                {
                    for (int32_t y = 0; y < mLevelObjects.height(); y++)
                    {
                        if (mLevelObjects[x][y].size() > 0)
                        {
                            mLevelObjects[x][y].clear();
                        }
                    }
                }
                fill(*state->level, state->mObjects, mLevelObjects);
                fill(*state->level, state->mItems, mItems);

                Render::drawLevel(state->level->mLevel,
                                  state->tileset.minTops->getCacheIndex(),
                                  state->tileset.minBottoms->getCacheIndex(),
                                  &mSpriteManager,
                                  mLevelObjects,
                                  mItems,
                                  state->mPos.current().first,
                                  state->mPos.current().second,
                                  state->mPos.next().first,
                                  state->mPos.next().second,
                                  state->mPos.getDist());
            }

            Render::drawGui(state->nuklearData, &mSpriteManager);
            {
                Renderer::drawCursor(state);
            }
        }

        Render::draw();

        I32sAs64 tmp;
        tmp.int32s[0] = Render::WIDTH;
        tmp.int32s[1] = Render::HEIGHT;

        mWidthHeightTmp = tmp.int64;

        return true;
    }

    void Renderer::drawCursor(RenderState* State)
    {

        if (!State->mCursorEmpty)
        {
            Render::Sprite sprite = mSpriteManager.get(State->mCursorSpriteGroup->getCacheIndex())->operator[](State->mCursorFrame);
            Render::spriteSize(sprite, mCursorSize.x, mCursorSize.y);
            Render::drawCursor(sprite, State->mCursorHotspot);
        }
        else
        {
            Render::drawCursor(NULL, State->mCursorHotspot);
        }
        return;
    }

    void Renderer::cleanup() { mSpriteManager.clear(); }

    void Renderer::getWindowDimensions(int32_t& w, int32_t& h)
    {
        I32sAs64 tmp;
        tmp.int64 = mWidthHeightTmp;

        w = tmp.int32s[0];
        h = tmp.int32s[1];
    }

    void Renderer::loadFonts(const DiabloExe::DiabloExe& exe)
    {
        mSmallTextFont = generateCelFont("ctrlpan/smaltext.cel", exe.getFontData("smaltext"), 1);
        mBigTGoldFont = generateCelFont("data/bigtgold.cel", exe.getFontData("bigtgold"), 2);
        for (auto size : {16, 24, 30, 42})
        {
            std::string prefix = "ui_art/font" + std::to_string(size);
            mGoldFont[size] = generateFont(prefix + "g.pcx", prefix + ".bin");
            if (size != 42)
                mSilverFont[size] = generateFont(prefix + "s.pcx", prefix + ".bin");
        }
    }

    bool Renderer::getAndClearSpritesNeedingPreloading(std::vector<uint32_t>& sprites) { return mSpriteManager.getAndClearSpritesNeedingPreloading(sprites); }

    nk_user_font* Renderer::smallFont() const { return &mSmallTextFont->nkFont; }

    nk_user_font* Renderer::bigTGoldFont() const { return &mBigTGoldFont->nkFont; }

    nk_user_font* Renderer::goldFont(int height) const { return &mGoldFont.at(height)->nkFont; }

    nk_user_font* Renderer::silverFont(int height) const { return &mSilverFont.at(height)->nkFont; }
}
