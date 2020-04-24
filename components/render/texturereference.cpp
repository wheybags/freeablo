#include <render/texture.h>
#include <render/texturereference.h>

namespace Render
{
    TextureReference::TextureReference(Render::Texture* standaloneTexture)
        : mWidth(standaloneTexture->width()), mHeight(standaloneTexture->height()), mTrimmedWidth(mWidth), mTrimmedHeight(mHeight), mTexture(standaloneTexture)
    {
    }

    bool TextureReference::isTrimmed() const
    {
        return mTrimmedWidth != mWidth || mTrimmedHeight != mHeight;
    }
}
