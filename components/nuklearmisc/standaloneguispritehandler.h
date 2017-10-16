#pragma once

#include <stdint.h>
#include <vector>
#include <fa_nuklear.h>
#include <map>
#include <render/render.h>
#include <input/inputmanager.h>

namespace NuklearMisc
{
    class StandaloneGuiHandler;

    class GuiSprite
    {
    public:
        GuiSprite(Render::SpriteGroup* sprite, uint32_t cacheIndex, StandaloneGuiHandler* handler);
        ~GuiSprite();

        struct nk_image getNkImage(int32_t frame)
        {
            return nk_image_handle(nk_handle_ptr(&mFrameIds[frame]));
        }

        Render::SpriteGroup* getSprite()
        {
            return mSprite;
        }

    private:
        Render::SpriteGroup* mSprite;
        StandaloneGuiHandler* mHandler;
        uint32_t mCacheIndex;

        struct id
        {
            uint32_t cacheIndex;
            uint32_t frameIndex;
        };

        std::vector<id> mFrameIds;
    };

    class StandaloneGuiHandler : private Render::SpriteCacheBase
    {
    public:
        StandaloneGuiHandler(const std::string& title, const Render::RenderSettings& renderSettings);
        ~StandaloneGuiHandler();

        GuiSprite* getSprite(Render::SpriteGroup* sprite);
        nk_context* getNuklearContext() { return &mCtx; }
        bool update();

    private:
        static void fontStashBegin(nk_font_atlas& atlas);
        nk_handle fontStashEnd(nk_font_atlas& atlas, nk_draw_null_texture& nullTex);

        Render::SpriteGroup* get(uint32_t key) override
        {
            return mSprites[key]->getSprite();
        }

        virtual void setImmortal(uint32_t, bool) override {}

        uint32_t mNextFrameId = 1;
        std::map<uint32_t, GuiSprite*> mSprites;

        Render::NuklearGraphicsContext mNuklearGraphicsContext;
        nk_context mCtx;
        Input::InputManager mInput;
        NuklearFrameDump mNuklearData;

        friend class GuiSprite;
    };
}
