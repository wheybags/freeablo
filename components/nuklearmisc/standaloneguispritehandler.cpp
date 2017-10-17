#include "standaloneguispritehandler.h"

#include "inputfwd.h"

namespace NuklearMisc
{
    GuiSprite::GuiSprite(Render::SpriteGroup* sprite, uint32_t cacheIndex, StandaloneGuiHandler* handler)
        : mSprite(sprite)
        , mHandler(handler)
        , mCacheIndex(cacheIndex)
    {
        for(uint32_t i = 0; i < sprite->size(); i++)
        {
            id id;
            id.cacheIndex = cacheIndex;
            id.frameIndex = i;

            mFrameIds.push_back(id);
        }
    }

    GuiSprite::~GuiSprite()
    {
        delete mSprite;
        mSprite = nullptr;

        mHandler->mSprites.erase(mCacheIndex);
    }

    StandaloneGuiHandler::StandaloneGuiHandler(const std::string& title, const Render::RenderSettings& renderSettings)
        : mInput([this] (Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(&mCtx, true, key, mInput.getModifiers()); },
                 [this] (Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(&mCtx, false, key, mInput.getModifiers()); },
                 [this] (int32_t x, int32_t y, Input::Key key, bool isDoubleClick) { NuklearMisc::handleNuklearMouseEvent(&mCtx, x, y, key, true, isDoubleClick); },
                 [this] (int32_t x, int32_t y, Input::Key key) { NuklearMisc::handleNuklearMouseEvent(&mCtx, x, y, key, false, false); },
                 [this] (int32_t x, int32_t y, int32_t xrel, int32_t yrel) { NuklearMisc::handleNuklearMouseMoveEvent(&mCtx, x, y, xrel, yrel); },
                 [this] (int32_t x, int32_t y) { NuklearMisc::handleNuklearMouseWheelEvent(&mCtx, x, y); },
                 [this] (std::string inp) { NuklearMisc::handleNuklearTextInputEvent(&mCtx, inp); })
    {
        nk_init_default(&mCtx, nullptr);
        mCtx.clip.copy = nullptr;// nk_sdl_clipbard_copy;
        mCtx.clip.paste = nullptr;// nk_sdl_clipbard_paste;
        mCtx.clip.userdata = nk_handle_ptr(0);

        Render::init(title, renderSettings, mNuklearGraphicsContext, &mCtx);

        // Load Cursor: if you uncomment cursor loading please hide the cursor
        {
            fontStashBegin(mNuklearGraphicsContext.atlas);
            //struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
            //struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
            //struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
            //struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
            //struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
            //struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
            mNuklearGraphicsContext.dev.font_tex = fontStashEnd(mNuklearGraphicsContext.atlas, mNuklearGraphicsContext.dev.null);
            //nk_style_load_all_cursors(ctx, atlas->cursors);
            //nk_style_set_font(ctx, &roboto->handle);
        }

        mNuklearData.init(mNuklearGraphicsContext.dev);
    }

    StandaloneGuiHandler::~StandaloneGuiHandler()
    {
        while(!mSprites.empty())
            delete mSprites.begin()->second;

        destroyNuklearGraphicsContext(mNuklearGraphicsContext);
        nk_free(&mCtx);

        Render::quit();
    }

    GuiSprite* StandaloneGuiHandler::getSprite(Render::SpriteGroup* sprite)
    {
        auto retval = new GuiSprite(sprite, mNextFrameId, this);
        mSprites[mNextFrameId] = retval;
        mNextFrameId++;
        return retval;
    }

    void StandaloneGuiHandler::fontStashBegin(nk_font_atlas& atlas)
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);
    }

    nk_handle StandaloneGuiHandler::fontStashEnd(nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
    {
        const void *image; int w, h;
        image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

        GuiSprite* sprite = getSprite(Render::loadSprite((uint8_t*)image, w, h));

        nk_handle handle = sprite->getNkImage(0).handle;
        nk_font_atlas_end(&atlas, handle, &nullTex);

        if (atlas.default_font)
            nk_style_set_font(&mCtx, &atlas.default_font->handle);

        return handle;
    }

    bool StandaloneGuiHandler::update()
    {
        nk_input_begin(&mCtx);
        mInput.poll();
        bool quit = mInput.processInput(false);
        nk_input_end(&mCtx);

        mNuklearData.fill(&mCtx);
        nk_clear(&mCtx);

        Render::clear();
        Render::drawGui(mNuklearData, this);
        Render::draw();

        return quit;
    }
}
