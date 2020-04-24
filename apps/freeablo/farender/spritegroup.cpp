#include "spritegroup.h"
#include <render/render.h>
#include <render/texture.h>
#include <render/texturereference.h>

namespace FARender
{
    FASpriteGroup::FASpriteGroup(std::vector<const Render::TextureReference*>&& spriteReferences, std::optional<int32_t> animationLength)
        : frameHandles(std::move(spriteReferences)), mOwnsFrameHandles(false)
    {
        if (animationLength)
            animLength = *animationLength;
        else
            animLength = frameHandles.size();
    }

    FASpriteGroup::FASpriteGroup(std::unique_ptr<Render::Texture>&& texture)
    {
        mStandaloneTexture = std::move(texture);
        animLength = 1;

        mOwnsFrameHandles = true;
        frameHandles.push_back(new Render::TextureReference(mStandaloneTexture.get()));
    }

    FASpriteGroup::~FASpriteGroup() = default;

    struct nk_image FASpriteGroup::getNkImage(int32_t frame)
    {
        release_assert(frame >= 0 && frame < (int32_t)frameHandles.size());

        auto ret = nk_subimage_handle(nk_handle_ptr((void*)frameHandles[frame]),
                                      this->getWidth(frame),
                                      this->getHeight(frame),
                                      nk_rect(0, 0, this->getWidth(frame), this->getHeight(frame)));
        return ret;
    }

    int32_t FASpriteGroup::getWidth(int32_t frame) const { return frameHandles[frame]->mWidth; }

    int32_t FASpriteGroup::getHeight(int32_t frame) const { return frameHandles[frame]->mHeight; }
}
