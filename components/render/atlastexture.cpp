#include "atlastexture.h"
#include "../../extern/RectangleBinPack/SkylineBinPack.h"
#include <memory>
#include <misc/assert.h>

/* Stores many small textures into a large texture (or array of textures)
 * to allow batched drawing commands that increase performance. */

namespace Render
{
    AtlasTexture::AtlasTexture()
    {
        GLint maxTextureSize;
        GLint maxArrayTextureLayers;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);

        // According to this source GL_MAX_TEXTURE_SIZE is still 8192 for a lot of devices:
        //      https://feedback.wildfiregames.com/report/opengl/feature/GL_MAX_TEXTURE_SIZE
        // A single 8192*8192 texture isn't quite enough to store all the sprites in a level,
        // so a 2D texture array with 2 layers is used.
        static const GLint requiredTextureSize = 8192;
        static const GLint requiredTextureLayers = 2;
        release_assert(maxTextureSize >= requiredTextureSize);
        release_assert(maxArrayTextureLayers >= requiredTextureLayers);
        mTextureWidth = requiredTextureSize;
        mTextureHeight = requiredTextureSize;
        mTextureLayers = requiredTextureLayers;

        // Atlas texture is currently packed so reduce alignment requirements.
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &mTextureArrayId);

        for (int32_t layer = 0; layer < mTextureLayers; layer++)
            mBinPacker.push_back(std::make_unique<rbp::SkylineBinPack>(mTextureWidth, mTextureHeight, false));

        bind();

        // Allocate memory for texture array (by passing NULL).
        // NOTE: For GL_COMPRESSED_RGBA image dimensions need to be padded to a
        // multiple/alignment of 4: https://forums.khronos.org/showthread.php/77554
        glTexImage3D(GL_TEXTURE_2D_ARRAY,
                     0,
                     /*GL_RGB5_A1*/ GL_RGBA8 /*GL_COMPRESSED_RGBA*/,
                     mTextureWidth,
                     mTextureHeight,
                     mTextureLayers,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     NULL);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        clear();

#ifdef DEBUG_ATLAS_TEXTURE
        GLint internalFormat = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
        printf("MaxTextureSize %d, used (%d, %d, %d), 0x%04X\n", maxTextureSize, mTextureWidth, mTextureHeight, mTextureLayers, internalFormat);
#endif
    }

    size_t AtlasTexture::addTexture(int32_t width, int32_t height, const void* data)
    {
        // Alignment of 4 is required for default pixel store alignment or for using compressed textures.
        // Padding is useful for avoiding artifacts when highlighting edges in a packed atlas texture.
        // Only the bottom and right need be padded when adding an image.
        // e.g. adding two 3x3 images beside one another (I=image, P=padding, A=alignment padding):
        // PADDING = 1        PADDING = 2            PADDING = 1        PADDING = 2
        // ALIGNMENT = 1      ALIGNMENT = 1          ALIGNMENT = 4      ALIGNMENT = 4
        // I I I P I I I P    I I I P P I I I P P    I I I P I I I P    I I I P P A A A I I I P P A A A
        // I I I P I I I P    I I I P P I I I P P    I I I P I I I P    I I I P P A A A I I I P P A A A
        // I I I P I I I P    I I I P P I I I P P    I I I P I I I P    I I I P P A A A I I I P P A A A
        // P P P P P P P P    P P P P P P P P P P    P P P P P P P P    P P P P P A A A P P P P P A A A
        //                    P P P P P P P P P P                       P P P P P A A A P P P P P A A A
        //                                                              A A A A A A A A A A A A A A A A
        //                                                              A A A A A A A A A A A A A A A A
        //                                                              A A A A A A A A A A A A A A A A
        static const int32_t PADDING = 1;   // Must be >= 0
        static const int32_t ALIGNMENT = 1; // Must be >= 1
        int32_t paddedWidth = (width + PADDING + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
        int32_t paddedHeight = (height + PADDING + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
        release_assert(paddedWidth <= mTextureWidth && paddedHeight <= mTextureHeight); // Texture size too small...

        rbp::Rect packedPos = rbp::Rect();
        int32_t layer;
        for (layer = 0; layer < mTextureLayers; layer++)
        {
            packedPos = mBinPacker[layer]->Insert(paddedWidth, paddedHeight, rbp::SkylineBinPack::LevelMinWasteFit);
            if (packedPos.height != 0)
                break;
        }
        release_assert(layer < mTextureLayers); // Run out of layers...

        bind();
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, packedPos.x, packedPos.y, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // NOTE: when using compressed textures sometimes adding a
        // glFlush/glFinish here fixes tiles a bit, not sure why..

        auto id = mNextTextureId++;
        mLookupMap[id] = AtlasTextureEntry(packedPos.x, packedPos.y, layer, width, height);

        return id;
    }

    void AtlasTexture::bind() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureArrayId);
    }

    void AtlasTexture::free() { glDeleteTextures(1, &mTextureArrayId); }

    float AtlasTexture::getOccupancy() const
    {
        float summedOccupancy = 0;
        for (auto& bp : mBinPacker)
            summedOccupancy += bp->Occupancy();
        return summedOccupancy / mBinPacker.size() * 100;
    }

    void AtlasTexture::clear()
    {
        for (int32_t layer = 0; layer < mTextureLayers; layer++)
            mBinPacker[layer]->Init(mTextureWidth, mTextureHeight, false);
        mNextTextureId = 1;

        // Clear the texture, we want any unused padded areas to be transparent (especially for highlighting edges).
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

        bind();

        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTextureArrayId, 0);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);
    }
}
