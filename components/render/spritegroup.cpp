#include <fa_nuklear.h>
#include <misc/assert.h>
#include <render/spritegroup.h>
#include <render/texture.h>

namespace Render
{
    SpriteGroup::SpriteGroup(std::vector<const Render::TextureReference*>&& spriteReferences, std::optional<int32_t> animationLength)
        : frameHandles(std::move(spriteReferences)), mOwnsFrameHandles(false)
    {
        if (animationLength)
            animLength = *animationLength;
        else
            animLength = frameHandles.size();
    }

    SpriteGroup::SpriteGroup(std::unique_ptr<Render::Texture>&& texture)
    {
        mStandaloneTexture = std::move(texture);
        animLength = 1;

        mOwnsFrameHandles = true;
        frameHandles.push_back(new Render::TextureReference(mStandaloneTexture.get()));
    }

    SpriteGroup::~SpriteGroup() = default;

    struct nk_image SpriteGroup::getNkImage(int32_t frame)
    {
        release_assert(frame >= 0 && frame < (int32_t)frameHandles.size());

        auto ret = nk_subimage_handle(nk_handle_ptr((void*)frameHandles[frame]),
                                      this->getWidth(frame),
                                      this->getHeight(frame),
                                      nk_rect(0, 0, this->getWidth(frame), this->getHeight(frame)));
        return ret;
    }

    int32_t SpriteGroup::getWidth(int32_t frame) const { return frameHandles[frame]->mWidth; }

    int32_t SpriteGroup::getHeight(int32_t frame) const { return frameHandles[frame]->mHeight; }
}
