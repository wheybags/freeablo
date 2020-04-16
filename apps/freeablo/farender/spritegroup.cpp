#include "spritegroup.h"
#include <render/render.h>
#include <render/texture.h>

namespace FARender
{
    void FASpriteGroup::init(std::unique_ptr<Render::SpriteGroup>&& realSpriteGroup)
    {
        mRealSpriteGroup = std::move(realSpriteGroup);

        animLength = mRealSpriteGroup->animLength();

        frameHandles.resize(animLength);
        for (uint32_t i = 0; i < frameHandles.size(); i++)
        {
            frameHandles[i].spriteGroup = mRealSpriteGroup.get();
            frameHandles[i].frameNumber = i;
        }
    }

    void FASpriteGroup::init(std::unique_ptr<Render::Texture>&& texture)
    {
        mStandaloneTexture = std::move(texture);
        animLength = 1;

        frameHandles.resize(1);
        frameHandles[0].texture = mStandaloneTexture.get();
        frameHandles[0].frameNumber = 0;
    }

    struct nk_image FASpriteGroup::getNkImage(int32_t frame)
    {
        release_assert(frame >= 0 && frame < (int32_t)frameHandles.size());
        auto ret = nk_subimage_handle(
            nk_handle_ptr(&frameHandles[frame]), this->getWidth(frame), this->getHeight(frame), nk_rect(0, 0, this->getWidth(frame), this->getHeight(frame)));
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

    FASpriteGroup::~FASpriteGroup() = default;
}
