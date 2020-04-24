#include "spritegroup.h"
#include <render/render.h>
#include <render/texture.h>

namespace FARender
{
    FASpriteGroup::FASpriteGroup(std::unique_ptr<Render::SpriteGroup>&& realSpriteGroup)
    {
        mRealSpriteGroup = std::move(realSpriteGroup);
        animLength = mRealSpriteGroup->animLength();
    }

    FASpriteGroup::FASpriteGroup(std::unique_ptr<Render::Texture>&& texture)
    {
        mStandaloneTexture = std::move(texture);
        animLength = 1;

        frameHandles.emplace_back(mStandaloneTexture.get());
    }

    FASpriteGroup::~FASpriteGroup() = default;

    struct nk_image FASpriteGroup::getNkImage(int32_t frame)
    {
        const Render::TextureReference* ref = nullptr;

        if (mRealSpriteGroup)
        {
            release_assert(frame >= 0 && frame < int32_t(mRealSpriteGroup->size()));
            ref = mRealSpriteGroup->operator[](frame);
        }
        else
        {
            release_assert(frame >= 0 && frame < (int32_t)frameHandles.size());
            ref = &frameHandles[frame];
        }

        auto ret = nk_subimage_handle(
            nk_handle_ptr((void*)ref), this->getWidth(frame), this->getHeight(frame), nk_rect(0, 0, this->getWidth(frame), this->getHeight(frame)));
        return ret;
    }

    int32_t FASpriteGroup::getAnimLength() const
    {
        if (mStandaloneTexture)
            return 1;
        return mRealSpriteGroup->animLength();
    }

    int32_t FASpriteGroup::getWidth(int32_t frame) const
    {
        if (mStandaloneTexture)
            return mStandaloneTexture->width();

        return mRealSpriteGroup->operator[](frame)->mWidth;
    }

    int32_t FASpriteGroup::getHeight(int32_t frame) const
    {
        if (mStandaloneTexture)
            return mStandaloneTexture->height();

        return mRealSpriteGroup->operator[](frame)->mHeight;
    }
}
