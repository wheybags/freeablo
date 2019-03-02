/* Stores many small textures into a large texture (or array of textures)
 * to allow batched drawing commands that increase performance.
 * Currently implemented as an array of 2D textures, would be simpler with much
 * better performance if a proper 2D texture array (GL_TEXTURE_2D_ARRAY) was
 * used. Author couldn't quite get it to work, parts of the Nuklear GUI would
 * be missing if the array texture was allocated with > 1 depth (layers)... */

#include "atlastexture.h"
#include "../../extern/RectangleBinPack/MaxRectsBinPack.h"

#include <boost/make_unique.hpp>
#include <misc/assert.h>

namespace Render
{
    AtlasTexture::AtlasTexture()
    {
        GLint maxTextureSize;
        GLint maxTextures = 8; // Hardcoded in fragment shader, GL 3.x minimum is 16.
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

        mTextureWidth = maxTextureSize;
        mTextureHeight = maxTextureSize;

        // Limit number of textures to a reasonable level (measured from testing).
        // Note: Increasing this has a severe impact on performance.
        GLint estimatedRequiredTextures = (1uLL << 29) / (mTextureWidth * mTextureHeight);
        mTextureLayers = std::min(estimatedRequiredTextures, maxTextures);

        // Atlas texture is currently packed so reduce alignment requirements.
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        mTextureArrayIds.resize(mTextureLayers);
        glGenTextures(mTextureLayers, &mTextureArrayIds[0]);

        for (int32_t layer = 0; layer < mTextureLayers; layer++)
            mBinPacker.push_back(boost::make_unique<rbp::MaxRectsBinPack>(mTextureWidth, mTextureHeight, false));

        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

        for (int i = 0; i < mTextureLayers; i++)
        {
            bind(i);

            // Allocate memory for texture array (by passing NULL).
            // NOTE: For GL_COMPRESSED_RGBA image dimensions need to be padded to a
            // multiple/alignment of 4: https://forums.khronos.org/showthread.php/77554
            // Also may need to clear texture if there are
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         /*GL_RGBA8*/ GL_RGB5_A1 /*GL_COMPRESSED_RGBA*/,
                         mTextureWidth,
                         mTextureHeight,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         NULL);

            // Clear the texture, it is undefined upon initialisation and we want any
            // unused padded areas to be transparent (especially for highlighting edges).
            glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTextureArrayIds[i], 0);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);

#ifdef DEBUG_ATLAS_TEXTURE
        GLint internalFormat = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
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

        // This assert may cause trouble as one of the Diablo 1 images is 10752 pixels high.
        // However ~46% (https://feedback.wildfiregames.com/report/opengl/feature/GL_MAX_TEXTURE_SIZE)
        // of graphics cards are limited to 8192, not quite sure how it ever worked..
        release_assert(paddedWidth <= mTextureWidth && paddedHeight <= mTextureHeight); // Texture size too small...

        rbp::Rect packedPos;
        int32_t layer;
        for (layer = 0; layer < mTextureLayers; layer++)
        {
            packedPos = mBinPacker[layer]->Insert(paddedWidth, paddedHeight, rbp::MaxRectsBinPack::RectBestAreaFit);
            release_assert(packedPos.height != 0);
            if (packedPos.height != 0)
                break;
        }
        release_assert(layer < mTextureLayers); // Run out of layers...

        bind(layer);
        glTexSubImage2D(GL_TEXTURE_2D, 0, packedPos.x, packedPos.y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // NOTE: when using compressed textures sometimes adding a
        // glFlush/glFinish here fixes tiles a bit, not sure why..

        auto id = mNextTextureId++;
        mLookupMap[id] = AtlasTextureEntry(packedPos.x, packedPos.y, layer, width, height);

        return id;
    }

    void AtlasTexture::bind(GLuint layer) const
    {
        glActiveTexture(GL_TEXTURE0 + layer);
        glBindTexture(GL_TEXTURE_2D, mTextureArrayIds[layer]);
    }

    void AtlasTexture::bind() const
    {
        for (int i = 0; i < mTextureLayers; i++)
            bind(i);
    }

    void AtlasTexture::free() { glDeleteTextures(mTextureLayers, &mTextureArrayIds[0]); }

    float AtlasTexture::getOccupancy() const
    {
        float summedOccupancy = 0;
        for (auto& bp : mBinPacker)
            summedOccupancy += bp->Occupancy();
        return summedOccupancy / mBinPacker.size() * 100;
    }
}
