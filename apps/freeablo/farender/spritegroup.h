#pragma once
#include <cstdint>
#include <fa_nuklear.h>
#include <nuklearmisc/nuklearframedump.h>
#include <render/texture.h>

namespace FARender
{
    class Renderer;
    class FASpriteGroup
    {
    public:
        FASpriteGroup(std::vector<const Render::TextureReference*>&& spriteReferences, std::optional<int32_t> animationLength);
        FASpriteGroup(std::unique_ptr<Render::Texture>&& texture);

        ~FASpriteGroup();

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
