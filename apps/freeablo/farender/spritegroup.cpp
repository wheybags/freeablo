#include "spritegroup.h"
#include <render/render.h>
#include <render/texture.h>

namespace FARender
{
    void FASpriteGroup::init(Render::SpriteGroup* realSpriteGroup)
    {
        mRealSpriteGroup = realSpriteGroup;
        animLength = mRealSpriteGroup->animLength();

        for (size_t i = 0; i < mRealSpriteGroup->size(); i++)
        {
            const Render::Sprite& sprite = mRealSpriteGroup->operator[](i);

            int32_t w, h;
            Render::spriteSize(sprite, w, h);

            width.push_back(w);
            height.push_back(h);
        }

        frameHandles.resize(animLength);
        for (uint32_t i = 0; i < frameHandles.size(); i++)
        {
            frameHandles[i].spriteGroup = realSpriteGroup;
            frameHandles[i].frameNumber = i;
        }
    }

    void FASpriteGroup::init(std::unique_ptr<Render::Texture>&& texture)
    {
        mStandaloneTexture = std::move(texture);
        animLength = 1;

        width.push_back(mStandaloneTexture->width());
        height.push_back(mStandaloneTexture->height());

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

    FASpriteGroup::~FASpriteGroup() = default;
}
