#pragma once
#include <fa_nuklear.h>
#include <memory>
#include <optional>
#include <render/texturereference.h>
#include <vector>

class Image;

namespace Render
{
    class SpriteGroup
    {
    public:
        SpriteGroup(std::vector<const Render::TextureReference*>&& spriteReferences, int32_t animationLength);
        explicit SpriteGroup(const std::vector<Image>& images);
        explicit SpriteGroup(Image&& image);
        explicit SpriteGroup(std::vector<std::unique_ptr<Render::Texture>>&& textures);
        explicit SpriteGroup(std::unique_ptr<Render::Texture>&& texture);

        SpriteGroup() = delete;
        SpriteGroup(SpriteGroup&) = delete;

        ~SpriteGroup();

        int32_t size() const { return int32_t(mTextureReferences.size()); }
        int32_t getAnimationLength() const { return animationLength; }
        int32_t getWidth(int32_t frame = 0) const;
        int32_t getHeight(int32_t frame = 0) const;

        const Render::TextureReference* getFrame(int32_t frame) const;
        struct nk_image getNkImage(int32_t frame = 0);

    private:
        std::vector<std::unique_ptr<Render::Texture>> mTextures;
        std::vector<const Render::TextureReference*> mTextureReferences;
        bool mOwnsTextureReferences = false;

        int32_t animationLength = 0;
    };
}
