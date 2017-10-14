#include "standaloneguispritehandler.h"

GuiSprite::GuiSprite(Render::SpriteGroup* sprite, uint32_t cacheIndex, FAStandaloneGuiSpriteHandler* handler)
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

GuiSprite* FAStandaloneGuiSpriteHandler::getCVSprite(Render::SpriteGroup* sprite)
{
    auto retval = new GuiSprite(sprite, mNextFrameId, this);
    mSprites[mNextFrameId] = retval;
    mNextFrameId++;
    return retval;
}

void FAStandaloneGuiSpriteHandler::nk_fa_font_stash_begin(nk_font_atlas& atlas)
{
    nk_font_atlas_init_default(&atlas);
    nk_font_atlas_begin(&atlas);
}

nk_handle FAStandaloneGuiSpriteHandler::nk_fa_font_stash_end(nk_context* ctx, nk_font_atlas& atlas, nk_draw_null_texture& nullTex)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);

    GuiSprite* sprite = getCVSprite(Render::loadSprite((uint8_t*)image, w, h));

    nk_handle handle = sprite->getNkImage(0).handle;
    nk_font_atlas_end(&atlas, handle, &nullTex);

    if (atlas.default_font)
        nk_style_set_font(ctx, &atlas.default_font->handle);

    return handle;
}
