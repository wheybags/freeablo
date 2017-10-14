#pragma once

#include <stdint.h>
#include <vector>
#include <fa_nuklear.h>
#include <map>
#include <render/render.h>

class FAStandaloneGuiSpriteHandler;

class GuiSprite
{
public:
    GuiSprite(Render::SpriteGroup* sprite, uint32_t cacheIndex, FAStandaloneGuiSpriteHandler* handler);
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
    FAStandaloneGuiSpriteHandler* mHandler;
    uint32_t mCacheIndex;

    struct id
    {
        uint32_t cacheIndex;
        uint32_t frameIndex;
    };

    std::vector<id> mFrameIds;
};

class FAStandaloneGuiSpriteHandler : public Render::SpriteCacheBase
{
public:
    Render::SpriteGroup* get(uint32_t key) override
    {
        return mSprites[key]->getSprite();
    }

    virtual void setImmortal(uint32_t, bool) override {}

    GuiSprite* getCVSprite(Render::SpriteGroup* sprite);

    void nk_fa_font_stash_begin(nk_font_atlas& atlas);
    nk_handle nk_fa_font_stash_end(nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex);

private:
    uint32_t mNextFrameId = 1;
    std::map<uint32_t, GuiSprite*> mSprites;

    friend class GuiSprite;
};
