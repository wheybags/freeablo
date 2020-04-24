#pragma once
#include "nuklearframedump.h"
#include <cstdint>
#include <fa_nuklear.h>
#include <input/inputmanager.h>
#include <map>
#include <render/render.h>
#include <render/texture.h>
#include <vector>

namespace NuklearMisc
{
    class StandaloneGuiHandler;

    class GuiSprite
    {
    public:
        explicit GuiSprite(const std::vector<Image>& images);
        explicit GuiSprite(Image&& image);
        explicit GuiSprite(std::vector<std::unique_ptr<Render::Texture>>&& textures);
        explicit GuiSprite(std::unique_ptr<Render::Texture>&& texture);
        ~GuiSprite();

        int32_t size() const { return int32_t(mFrameIds.size()); }
        struct nk_image getNkImage(int32_t frame = 0);

    private:
        std::vector<std::unique_ptr<Render::Texture>> mTextures;
        std::vector<Render::TextureReference> mFrameIds;
    };

    class StandaloneGuiHandler
    {
    public:
        StandaloneGuiHandler(const std::string& title, const Render::RenderSettings& renderSettings);
        ~StandaloneGuiHandler();

        nk_context* getNuklearContext() { return &mCtx; }
        bool update();

    private:
        static void fontStashBegin(nk_font_atlas& atlas);
        static std::unique_ptr<GuiSprite> fontStashEnd(nk_context* ctx, NuklearDevice::InitData& initData);

        std::unique_ptr<NuklearDevice> mNuklearGraphicsContext;
        std::unique_ptr<GuiSprite> mNuklearFontTexture;

        nk_context mCtx = {};
        Input::InputManager mInput;
        std::unique_ptr<NuklearFrameDump> mNuklearData;

        friend class GuiSprite;
    };
}
