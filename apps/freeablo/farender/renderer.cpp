#include "renderer.h"
#include "../engine/threadmanager.h"
#include "../fagui/guimanager.h"
#include "../faworld/gamelevel.h"
#include "cel/celdecoder.h"
#include "cel/celfile.h"
#include "fontinfo.h"
#include "levelrenderer.h"
#include <Image/image.h>
#include <audio/fa_audio.h>
#include <misc/assert.h>
#include <numeric>
#include <render/cursor.h>
#include <render/renderinstance.h>
#include <render/spritegroup.h>
#include <render/texture.h>
#include <thread>

namespace FARender
{
    Renderer* Renderer::mRenderer = nullptr;

    std::unique_ptr<CelFontInfo> Renderer::generateCelFont(Render::SpriteGroup* fontTexture, const DiabloExe::FontData& fontData, int spacing)
    {
        std::unique_ptr<CelFontInfo> ret(new CelFontInfo());
        ret->initByFontData(fontData, fontTexture->getWidth(), spacing);
        ret->nkFont.userdata.ptr = ret.get();
        ret->nkFont.height = fontTexture->getHeight();
        ret->nkFont.width = &CelFontInfo::getWidth;
        ret->nkFont.query = &CelFontInfo::queryGlyph;
        ret->nkFont.texture = fontTexture->getNkImage().handle;
        return ret;
    }

    std::unique_ptr<PcxFontInfo> Renderer::generateFont(Render::SpriteGroup* fontTexture, const std::string& binPath, const PcxFontInitData& fontInitData)
    {
        std::unique_ptr<PcxFontInfo> ret(new PcxFontInfo());
        ret->init(binPath, fontInitData);
        ret->nkFont.userdata.ptr = ret.get();
        ret->nkFont.height = fontInitData.spacingY;
        ret->nkFont.width = &PcxFontInfo::getWidth;
        ret->nkFont.query = &PcxFontInfo::queryGlyph;
        ret->nkFont.texture = fontTexture->getNkImage().handle;
        return ret;
    }

    Renderer* Renderer::get() { return mRenderer; }

    void nk_fa_font_stash_begin(nk_font_atlas& atlas)
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);
    }

    std::unique_ptr<Render::SpriteGroup> nk_fa_font_stash_end(nk_context* ctx, NuklearDevice::InitData& initData)
    {
        int width, height;
        const void* imageData = nk_font_atlas_bake(&initData.atlas, &width, &height, NK_FONT_ATLAS_RGBA32);

        std::unique_ptr<Render::SpriteGroup> sprite;
        {
            Render::BaseTextureInfo textureInfo;
            textureInfo.width = width;
            textureInfo.height = height;
            textureInfo.format = Render::Format::RGBA8UNorm;
            std::unique_ptr<Render::Texture> texture = Render::mainRenderInstance->createTexture(textureInfo);
            texture->updateImageData(0, 0, 0, texture->width(), texture->height(), reinterpret_cast<const uint8_t*>(imageData), texture->width());

            sprite = std::make_unique<Render::SpriteGroup>(std::move(texture));
        }

        nk_font_atlas_end(&initData.atlas, sprite->getNkImage().handle, &initData.nullTexture);

        if (initData.atlas.default_font)
            nk_style_set_font(ctx, &initData.atlas.default_font->handle);

        return sprite;
    }

    Renderer::Renderer(const DiabloExe::DiabloExe& exe, int32_t windowWidth, int32_t windowHeight, bool fullscreen)
        : mSpriteLoader(exe), mDone(false), mWidthHeightTmp(0)
    {
        release_assert(!mRenderer); // singleton, only one instance

        Render::RenderSettings settings = {};
        settings.windowWidth = windowWidth;
        settings.windowHeight = windowHeight;
        settings.fullscreen = fullscreen;

        Render::init("Freeablo", settings);

        // setup gui
        {
            nk_init_default(&mNuklearContext, nullptr);
            mNuklearContext.clip.copy = nullptr;  // nk_sdl_clipboard_copy;
            mNuklearContext.clip.paste = nullptr; // nk_sdl_clipboard_paste;
            mNuklearContext.clip.userdata = nk_handle_ptr(0);

            NuklearDevice::InitData initData;

            nk_fa_font_stash_begin(initData.atlas);
            // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
            mNuklearFontTexture = nk_fa_font_stash_end(&mNuklearContext, initData);
            mNuklearGraphicsData = std::make_unique<NuklearDevice>(*Render::mainRenderInstance, std::move(initData));
        }

        mStates.reserve(NUM_RENDER_STATES);
        for (size_t i = 0; i < NUM_RENDER_STATES; ++i)
            mStates.emplace_back(*mNuklearGraphicsData);

        mRenderer = this;

        mLevelRenderer = std::make_unique<LevelRenderer>();

        std::vector<Image> itemCursorImages = Cel::CelDecoder(mSpriteLoader.mGuiSprites.itemCursors.path).decode();
        mDefaultCursor = std::make_unique<Render::Cursor>(itemCursorImages[0], 0, 0);
    }

    Renderer::~Renderer()
    {
        mRenderer = nullptr;
        nk_free(&mNuklearContext);

        Render::quit();
    }

    void Renderer::stop() { mDone = true; }

    Tileset Renderer::getTileset(const FAWorld::GameLevel& gameLevel)
    {
        const Level::Level& level = gameLevel.mLevel;

        Tileset tileset;
        tileset.minTops = mSpriteLoader.getSprite(mSpriteLoader.mTilesetTops[level.getTilesetId()]);
        tileset.minBottoms = mSpriteLoader.getSprite(mSpriteLoader.mTilesetBottoms[level.getTilesetId()]);
        // Special Cels may not exist for certain levels.
        tileset.mSpecialSprites = nullptr;
        if (mSpriteLoader.mTilesetSpecials.count(level.getTilesetId()) != 0)
            tileset.mSpecialSprites = mSpriteLoader.getSprite(mSpriteLoader.mTilesetSpecials[level.getTilesetId()]);
        tileset.mSpecialSpriteMap = level.getSpecialCelMap();
        return tileset;
    }

    RenderState* Renderer::getFreeState()
    {
        for (size_t i = 0; i < mStates.size(); i++)
        {
            if (mStates[i].ready)
            {
                mStates[i].ready = false;
                return &mStates[i];
            }
        }

        return nullptr;
    }

    void Renderer::setCurrentState(RenderState* current) { Engine::ThreadManager::get()->sendRenderState(current); }

    Render::Tile Renderer::getTileByScreenPos(size_t x, size_t y, const FAWorld::Position& screenPos)
    {
        return mLevelRenderer->getTileByScreenPos(x, y, screenPos.getFractionalPos());
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

    static void fill(const FAWorld::GameLevel& level, const std::vector<ObjectToRender> src, LevelObjects& dst)
    {
        if (dst.width() != level.width() || dst.height() != level.height())
            dst = LevelObjects(level.width(), level.height());

        for (int32_t x = 0; x < dst.width(); x++)
            for (int32_t y = 0; y < dst.height(); y++)
                dst.get(x, y).clear();

        for (size_t i = 0; i < src.size(); i++)
        {
            auto& object = src[i];
            auto& position = object.position;

            LevelObject obj;
            obj.sprite = object.spriteGroup;
            obj.spriteFrame = object.frame;
            obj.fractionalPos = position.getFractionalPos();
            obj.hoverColor = object.hoverColor;
            obj.valid = true;

            int32_t x = position.current().x;
            int32_t y = position.current().y;
            dst.get(x, y).push_back(std::move(obj));
        }
    }

    bool Renderer::renderFrame(RenderState* state)
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

        if (state)
        {
            if (state->level)
            {
                fill(*state->level, state->mObjects, mLevelObjects);
                fill(*state->level, state->mItems, mItems);

                mLevelRenderer->drawLevel(state->level->mLevel,
                                          state->tileset.minTops,
                                          state->tileset.minBottoms,
                                          state->tileset.mSpecialSprites ? state->tileset.mSpecialSprites : nullptr,
                                          state->tileset.mSpecialSpriteMap,
                                          mLevelObjects,
                                          mItems,
                                          state->mPos.getFractionalPos(),
                                          state->debugData);
            }

            state->nuklearData.render({Render::getWindowSize().windowWidth, Render::getWindowSize().windowHeight}, *Render::mainCommandQueue);
            Renderer::updateCursor(state->currentCursor);
        }

        Render::draw();

        I32sAs64 tmp;
        tmp.int32s[0] = Render::WIDTH;
        tmp.int32s[1] = Render::HEIGHT;

        mWidthHeightTmp = tmp.int64;

        return true;
    }

    void Renderer::updateCursor(const Render::Cursor* cursor)
    {
        if (!cursor)
            Render::Cursor::setDefaultCursor();
        else if (cursor != mCurrentCursor)
            cursor->activateCursor();

        mCurrentCursor = cursor;
    }

    void Renderer::getWindowDimensions(int32_t& w, int32_t& h)
    {
        I32sAs64 tmp;
        tmp.int64 = mWidthHeightTmp;

        w = tmp.int32s[0];
        h = tmp.int32s[1];
    }

    void Renderer::loadFonts(const DiabloExe::DiabloExe& exe)
    {
        mSmallTextFont = generateCelFont(mSpriteLoader.getSprite(mSpriteLoader.mGuiSprites.smallTextFont), exe.getFontData("smaltext"), 1);
        mBigTGoldFont = generateCelFont(mSpriteLoader.getSprite(mSpriteLoader.mGuiSprites.bigTGoldFont), exe.getFontData("bigtgold"), 2);

        // Font textures are resized as they are very tall (some GPUs don't support textures larger than 8192x8192).
        // Resizing the font image also transposes the sub images (LayoutOrder vertical -> horizontal) in this case i.e.
        //      1     resize ->     1   2   3   4
        //      2                   5   6   7   8
        //      3                   9   ...
        //      ...
        PcxFontInitData fontInitData[] = {{16, 256, 256, 16, 16, PcxFontInitData::LayoutOrder::horizontal},
                                          {24, 384, 416, 24, 26, PcxFontInitData::LayoutOrder::horizontal},
                                          {30, 512, 496, 32, 31, PcxFontInitData::LayoutOrder::horizontal},
                                          {42, 640, 672, 40, 42, PcxFontInitData::LayoutOrder::horizontal}};

        for (size_t i = 0; i < sizeof(fontInitData) / sizeof(*fontInitData); i++)
        {
            const PcxFontInitData& initData = fontInitData[i];
            std::string prefix = "ui_art/font" + std::to_string(initData.fontSize);

            mGoldFont[initData.fontSize] = generateFont(mSpriteLoader.getSprite(*((&mSpriteLoader.mGuiSprites.fontGold16) + i)), prefix + ".bin", initData);
            if (initData.fontSize != 42)
                mSilverFont[initData.fontSize] =
                    generateFont(mSpriteLoader.getSprite(*((&mSpriteLoader.mGuiSprites.fontSilver16) + i)), prefix + ".bin", initData);
        }
    }

    nk_user_font* Renderer::smallFont() const { return &mSmallTextFont->nkFont; }

    nk_user_font* Renderer::bigTGoldFont() const { return &mBigTGoldFont->nkFont; }

    nk_user_font* Renderer::goldFont(int height) const { return &mGoldFont.at(height)->nkFont; }

    nk_user_font* Renderer::silverFont(int height) const { return &mSilverFont.at(height)->nkFont; }
}
