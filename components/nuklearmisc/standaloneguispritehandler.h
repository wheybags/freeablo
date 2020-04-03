#pragma once
#include <fa_nuklear.h>
#include <input/inputmanager.h>
#include <map>
#include <render/render.h>
#include <stdint.h>
#include <vector>

namespace NuklearMisc
{
    class StandaloneGuiHandler;

    class GuiSprite
    {
    public:
        explicit GuiSprite(Render::SpriteGroup* sprite);
        explicit GuiSprite(std::unique_ptr<Render::Texture>&& texture);
        ~GuiSprite();

        struct nk_image getNkImage(int32_t frame = 0) { return nk_image_handle(nk_handle_ptr(&mFrameIds[frame])); }

        Render::SpriteGroup* getSprite() { return mSprite.get(); }

    private:
        std::unique_ptr<Render::SpriteGroup> mSprite;
        std::unique_ptr<Render::Texture> mTexture;

        std::vector<FANuklearTextureHandle> mFrameIds;
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
        std::unique_ptr<GuiSprite> fontStashEnd(nk_font_atlas& atlas, nk_draw_null_texture& nullTex);

        Render::NuklearGraphicsContext mNuklearGraphicsContext;
        std::unique_ptr<GuiSprite> mNuklearFontTexture;

        nk_context mCtx = {};
        Input::InputManager mInput;
        NuklearFrameDump mNuklearData;

        friend class GuiSprite;
    };
}
