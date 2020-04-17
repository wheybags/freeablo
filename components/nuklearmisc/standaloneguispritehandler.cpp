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
            mFrameIds[i].texture = mTextures[i].get();
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
        Render::init(title, renderSettings);

        nk_init_default(&mCtx, nullptr);
        mCtx.clip.copy = nullptr;  // nk_sdl_clipboard_copy;
        mCtx.clip.paste = nullptr; // nk_sdl_clipboard_paste;
        mCtx.clip.userdata = nk_handle_ptr(0);

        NuklearDevice::InitData initData;

        fontStashBegin(initData.atlas);
        // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
        mNuklearFontTexture = fontStashEnd(&mCtx, initData);
        mNuklearGraphicsContext = std::make_unique<NuklearDevice>(*Render::mainRenderInstance, std::move(initData));

        mNuklearData = std::make_unique<NuklearFrameDump>(*mNuklearGraphicsContext);
    }

    StandaloneGuiHandler::~StandaloneGuiHandler()
    {
        nk_free(&mCtx);
        Render::quit();
    }

    void StandaloneGuiHandler::fontStashBegin(nk_font_atlas& atlas)
    {
        nk_font_atlas_init_default(&atlas);
        nk_font_atlas_begin(&atlas);
    }

    std::unique_ptr<GuiSprite> StandaloneGuiHandler::fontStashEnd(nk_context* ctx, NuklearDevice::InitData& initData)
    {
        const void* imageData;
        int width, height;
        imageData = nk_font_atlas_bake(&initData.atlas, &width, &height, NK_FONT_ATLAS_RGBA32);

        std::unique_ptr<GuiSprite> sprite;
        {
            Render::BaseTextureInfo textureInfo;
            textureInfo.width = width;
            textureInfo.height = height;
            textureInfo.format = Render::Format::RGBA8UNorm;
            std::unique_ptr<Render::Texture> texture = Render::mainRenderInstance->createTexture(textureInfo);
            texture->updateImageData(0, 0, 0, texture->width(), texture->height(), reinterpret_cast<const uint8_t*>(imageData));

            sprite = std::make_unique<GuiSprite>(std::move(texture));
        }

        nk_font_atlas_end(&initData.atlas, sprite->getNkImage().handle, &initData.nullTexture);

        if (initData.atlas.default_font)
            nk_style_set_font(ctx, &initData.atlas.default_font->handle);

        return sprite;
    }

    bool StandaloneGuiHandler::update()
    {
        nk_input_begin(&mCtx);
        mInput.poll();
        bool quit = mInput.processInput();
        nk_input_end(&mCtx);

        mNuklearData->fill(&mCtx);
        nk_clear(&mCtx);

        Render::clear();
        mNuklearData->render({Render::getWindowSize().windowWidth, Render::getWindowSize().windowHeight}, *Render::mainCommandQueue);
        Render::draw();

        return quit;
    }
}
