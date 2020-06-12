#include <fa_nuklear.h>
#include <render/texture.h>
#include <render/texturereference.h>

namespace Render
{
    TextureReference::TextureReference(Render::Texture* standaloneTexture)
        : mWidth(standaloneTexture->width()), mHeight(standaloneTexture->height()), mTrimmedWidth(mWidth), mTrimmedHeight(mHeight), mTexture(standaloneTexture)
    {
    }

    bool TextureReference::isTrimmed() const { return mTrimmedWidth != mWidth || mTrimmedHeight != mHeight; }

    struct nk_image TextureReference::getNkImage() const
    {
        return nk_subimage_handle(nk_handle_ptr((void*)this), mWidth, mHeight, nk_rect(0, 0, mWidth, mHeight));
    }
}
