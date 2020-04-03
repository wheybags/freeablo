#pragma once
#include <cstdint>
#include <fa_nuklear.h>
#include <render/nuklear_sdl_gl3.h>
#include <render/texture.h>

namespace FARender
{
    class Renderer;
    class FASpriteGroup
    {
    public:
        ~FASpriteGroup();

        void init(Render::SpriteGroup* realSpriteGroup);
        void init(std::unique_ptr<Render::Texture>&& texture);

        bool isValid() const { return mRealSpriteGroup || mStandaloneTexture.get(); }
        int32_t getAnimLength() const { return animLength; }
        int32_t getWidth(int frame = 0) const { return width[frame]; }
        int32_t getHeight(int frame = 0) const { return height[frame]; }
        struct nk_image getNkImage(int32_t frame = 0);

        Render::SpriteGroup* getSpriteGroup() { return mRealSpriteGroup; }

    private:
        int32_t animLength = 0;
        std::vector<int32_t> width = {};
        std::vector<int32_t> height = {};

        Render::SpriteGroup* mRealSpriteGroup = nullptr;
        std::unique_ptr<Render::Texture> mStandaloneTexture;

        std::vector<FANuklearTextureHandle> frameHandles;
    };
}
