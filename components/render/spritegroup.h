#pragma once
#include <memory>
#include <optional>
#include <render/texturereference.h>
#include <vector>

namespace Render
{
    class SpriteGroup
    {
    public:
        SpriteGroup(std::vector<const Render::TextureReference*>&& spriteReferences, std::optional<int32_t> animationLength);
        SpriteGroup(std::unique_ptr<Render::Texture>&& texture);

        ~SpriteGroup();

        size_t size() { return frameHandles.size(); }
        int32_t getAnimLength() const { return animLength; }
        int32_t getWidth(int32_t frame = 0) const;
        int32_t getHeight(int32_t frame = 0) const;

        const Render::TextureReference* getFrame(int32_t frame) { return frameHandles[frame]; }
        struct nk_image getNkImage(int32_t frame = 0);

    private:
        int32_t animLength = 0;

        std::unique_ptr<Render::Texture> mStandaloneTexture;

        std::vector<const Render::TextureReference*> frameHandles;
        bool mOwnsFrameHandles = false;
    };
}
