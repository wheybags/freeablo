#include <Image/image.h>
#include <fa_nuklear.h>
#include <misc/assert.h>
#include <misc/misc.h>
#include <render/render.h>
#include <render/renderinstance.h>
#include <render/spritegroup.h>
#include <render/texture.h>

namespace Render
{
    SpriteGroup::SpriteGroup(std::vector<const Render::TextureReference*>&& spriteReferences, int32_t animationLength)
        : mTextureReferences(std::move(spriteReferences)), mOwnsTextureReferences(false), animationLength(animationLength)
    {
    }

    SpriteGroup::SpriteGroup(std::vector<std::unique_ptr<Render::Texture>>&& textures) : mTextures(std::move(textures))
    {
        animationLength = int32_t(mTextures.size());

        mOwnsTextureReferences = true;
        mTextureReferences.reserve(mTextures.size());
        for (uint32_t i = 0; i < mTextures.size(); i++)
            mTextureReferences.emplace_back(new TextureReference(mTextures[i].get()));
    }

    SpriteGroup::SpriteGroup(std::unique_ptr<Render::Texture>&& texture) : SpriteGroup(moveToVector(std::move(texture))) {}

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
            texture->updateImageData(0, 0, 0, image.width(), image.height(), reinterpret_cast<const uint8_t*>(image.mData.data()), image.width());
            retval.emplace_back(texture.release());
        }

        return retval;
    }

    SpriteGroup::SpriteGroup(const std::vector<Image>& images) : SpriteGroup(imagesToTextures(images)) {}

    SpriteGroup::SpriteGroup(Image&& image) : SpriteGroup(moveToVector(std::move(image))) {}

    SpriteGroup::~SpriteGroup()
    {
        if (mOwnsTextureReferences)
            for (auto& handle : mTextureReferences)
                delete handle;
    }

    const Render::TextureReference* SpriteGroup::getFrame(int32_t frame) const
    {
        release_assert(frame >= 0 && frame < (int32_t)mTextureReferences.size());
        return mTextureReferences[frame];
    }

    struct nk_image SpriteGroup::getNkImage(int32_t frame)
    {
        release_assert(frame >= 0 && frame < (int32_t)mTextureReferences.size());
        return mTextureReferences[frame]->getNkImage();
    }

    int32_t SpriteGroup::getWidth(int32_t frame) const { return mTextureReferences[frame]->mWidth; }

    int32_t SpriteGroup::getHeight(int32_t frame) const { return mTextureReferences[frame]->mHeight; }
}
