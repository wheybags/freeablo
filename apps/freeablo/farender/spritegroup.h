#pragma once
#include <cstdint>
#include <fa_nuklear.h>
#include <nuklearmisc/nuklearframedump.h>
#include <render/misc.h>
#include <render/texture.h>

namespace FARender
{
    class Renderer;
    class FASpriteGroup
    {
    public:
        ~FASpriteGroup();

        void init(std::unique_ptr<Render::SpriteGroup>&& realSpriteGroup);
        void init(std::unique_ptr<Render::Texture>&& texture);

        bool isValid() const { return mRealSpriteGroup || mStandaloneTexture.get(); }

        int32_t getAnimLength() const;
        int32_t getWidth(int32_t frame = 0) const;
        int32_t getHeight(int32_t frame = 0) const;
        struct nk_image getNkImage(int32_t frame = 0);

        Render::SpriteGroup* getSpriteGroup()
        {
            debug_assert(mRealSpriteGroup);
            return mRealSpriteGroup.get();
        }

    private:
        int32_t animLength = 0;

        std::unique_ptr<Render::SpriteGroup> mRealSpriteGroup = nullptr;
        std::unique_ptr<Render::Texture> mStandaloneTexture;

        std::vector<FANuklearTextureHandle> frameHandles;
    };
}
