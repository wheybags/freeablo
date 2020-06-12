#pragma once
#include <cstdint>

struct nk_image;

namespace FARender
{
    class SpriteLoader;
}

namespace Render
{
    class Texture;

    // Represents a texture, which may or may not be packed into an atlas
    class TextureReference
    {
    public:
        explicit TextureReference(Render::Texture* standaloneTexture);
        TextureReference(TextureReference& other) = delete;
        TextureReference() = delete;
        struct nk_image getNkImage() const;

        bool isTrimmed() const;

    public:
        // offset in the atlas (or 0, if the texture is not in an atlas)
        int32_t mX = 0;
        int32_t mY = 0;

        // The original width and height of the texture.
        // Will be the same as trimmedW/H if the texture is not in an atlas.
        int32_t mWidth = 0;
        int32_t mHeight = 0;

        // Sometimes textures in atlases have the transparent pixels around their edges trimmed.
        // This is the amount of pixels removed from the left and top, if applicable.
        int32_t mTrimmedOffsetX = 0;
        int32_t mTrimmedOffsetY = 0;

        int32_t mTrimmedWidth = 0;
        int32_t mTrimmedHeight = 0;

        Render::Texture* mTexture = nullptr;

    protected:
        struct Tag
        {
        };
        explicit TextureReference(Tag) {}
        friend class AtlasTexture;
        friend class FARender::SpriteLoader;
    };
}
