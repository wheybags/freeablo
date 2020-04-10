#include "standaloneguispritehandler.h"
#include "inputfwd.h"
#include <render/renderinstance.h>
#include <render/texture.h>

namespace NuklearMisc
{
    GuiSprite::GuiSprite(std::vector<std::unique_ptr<Render::Texture>>&& textures) : mTextures(std::move(textures))
    {
        mFrameIds.resize(mTextures.size());
        for (uint32_t i = 0; i < mTextures.size(); i++)
        {
            mFrameIds[i].texture = mTextures[i].get();
            mFrameIds[i].frameNumber = i;
        }
    }

    GuiSprite::GuiSprite(std::unique_ptr<Render::Texture>&& texture) : GuiSprite(moveToVector(std::move(texture))) {}

    static std::vector<std::unique_ptr<Render::Texture>> imagesToTextures(const std::vector<Image>& images)
    {
        std::vector<std::unique_ptr<Render::Texture>> retval;
        retval.reserve(images.size());

        for (const auto& image : images)
        {
            Render::BaseTextureInfo textureInfo;
            textureInfo.width = image.width();
            textureInfo.height = image.height();
            textureInfo.forceTextureToBeATextureArray = true;
            textureInfo.format = Render::Format::RGBA8UNorm;
            std::unique_ptr<Render::Texture> texture = Render::mainRenderInstance->createTexture(textureInfo);
            texture->updateImageData(0, 0, 0, image.width(), image.height(), reinterpret_cast<const uint8_t*>(image.mData.data()));
            retval.emplace_back(texture.release());
        }

        return retval;
    }

    GuiSprite::GuiSprite(const std::vector<Image>& images) : GuiSprite(imagesToTextures(images)) {}

    GuiSprite::GuiSprite(Image&& image) : GuiSprite(moveToVector(std::move(image))) {}

    struct nk_image GuiSprite::getNkImage(int32_t frame)
    {
        struct nk_image image = nk_image_handle(nk_handle_ptr(&mFrameIds[frame]));

        image.w = mTextures[frame]->width();
        image.h = mTextures[frame]->height();
        image.region[0] = 0;
        image.region[1] = 1;
        image.region[2] = image.w;
        image.region[3] = image.h;

        return image;
    }

    GuiSprite::~GuiSprite() = default;

    StandaloneGuiHandler::StandaloneGuiHandler(const std::string& title, const Render::RenderSettings& renderSettings)
        : mInput(
              [this](Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(&mCtx, true, key, mInput.getModifiers()); },
              [this](Input::Key key) { NuklearMisc::handleNuklearKeyboardEvent(&mCtx, false, key, mInput.getModifiers()); },
              [this](int32_t x, int32_t y, Input::Key key, bool isDoubleClick) { NuklearMisc::handleNuklearMouseEvent(&mCtx, x, y, key, true, isDoubleClick); },
              [this](int32_t x, int32_t y, Input::Key key) { NuklearMisc::handleNuklearMouseEvent(&mCtx, x, y, key, false, false); },
              [this](int32_t x, int32_t y, int32_t xrel, int32_t yrel) { NuklearMisc::handleNuklearMouseMoveEvent(&mCtx, x, y, xrel, yrel); },
              [this](int32_t x, int32_t y) { NuklearMisc::handleNuklearMouseWheelEvent(&mCtx, x, y); },
              [this](std::string inp) { NuklearMisc::handleNuklearTextInputEvent(&mCtx, inp); })
    {
        nk_init_default(&mCtx, nullptr);
        mCtx.clip.copy = nullptr;  // nk_sdl_clipbard_copy;
        mCtx.clip.paste = nullptr; // nk_sdl_clipbard_paste;
        mCtx.clip.userdata = nk_handle_ptr(0);

        Render::init(title, renderSettings, mNuklearGraphicsContext, &mCtx);

        // Load Cursor: if you uncomment cursor loading please hide the cursor
        {
            fontStashBegin(mNuklearGraphicsContext.atlas);
            // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
            // struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
            // struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
            // struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
            // struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
            // struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
            mNuklearFontTexture = fontStashEnd(mNuklearGraphicsContext.atlas, mNuklearGraphicsContext.dev.null);
            mNuklearGraphicsContext.dev.font_tex = mNuklearFontTexture->getNkImage().handle;
            // nk_style_load_all_cursors(ctx, atlas->cursors);
            // nk_style_set_font(ctx, &roboto->handle);
        }

        mNuklearData.init(mNuklearGraphicsContext.dev);
    }

    StandaloneGuiHandler::~StandaloneGuiHandler()
    {
        destroyNuklearGraphicsContext(mNuklearGraphicsContext);
        nk_free(&mCtx);

        Render::quit();
    }

    void StandaloneGuiHandler::fontStashBegin(nk_font_atlas& atlas)
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);
    }

    std::unique_ptr<GuiSprite> StandaloneGuiHandler::fontStashEnd(nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
    {
        const void* imageData;
        int width, height;
        imageData = nk_font_atlas_bake(&atlas, &width, &height, NK_FONT_ATLAS_RGBA32);

        std::unique_ptr<GuiSprite> sprite;
        {
            Render::BaseTextureInfo textureInfo;
            textureInfo.width = width;
            textureInfo.height = height;
            textureInfo.forceTextureToBeATextureArray = true;
            textureInfo.format = Render::Format::RGBA8UNorm;
            std::unique_ptr<Render::Texture> texture = Render::mainRenderInstance->createTexture(textureInfo);
            texture->updateImageData(0, 0, 0, texture->width(), texture->height(), reinterpret_cast<const uint8_t*>(imageData));

            sprite = std::make_unique<GuiSprite>(std::move(texture));
        }

        nk_font_atlas_end(&atlas, sprite->getNkImage().handle, &nullTex);

        if (atlas.default_font)
            nk_style_set_font(&mCtx, &atlas.default_font->handle);

        return sprite;
    }

    bool StandaloneGuiHandler::update()
    {
        nk_input_begin(&mCtx);
        mInput.poll();
        bool quit = mInput.processInput();
        nk_input_end(&mCtx);

        mNuklearData.fill(&mCtx);
        nk_clear(&mCtx);

        Render::clear();
        Render::drawGui(mNuklearData);
        Render::draw();

        return quit;
    }
}
