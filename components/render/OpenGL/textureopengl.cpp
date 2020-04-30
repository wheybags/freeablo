#include <render/OpenGL/renderinstanceopengl.h>
#include <render/OpenGL/textureopengl.h>

namespace Render
{
    TextureOpenGL::TextureOpenGL(RenderInstanceOpenGL& instance, const BaseTextureInfo& info) : super(instance, info)
    {
        release_assert(!(isTextureArray() && mInfo.format == Format::Depth24Stencil8));

        glGenTextures(1, &mId);

        GLint internalFormat = 0;
        GLenum format = 0;
        GLenum type = 0;

        switch (mInfo.format)
        {
            case Format::RGBA8UNorm:
                internalFormat = GL_RGBA8;
                format = GL_RGBA;
                type = GL_UNSIGNED_BYTE;
                break;
            case Format::RGBA32F:
                internalFormat = GL_RGBA32F;
                format = GL_RGBA;
                type = GL_FLOAT;
                break;
            case Format::RGB32F:
                internalFormat = GL_RGB32F;
                format = GL_RGB;
                type = GL_FLOAT;
                break;
            case Format::RG32F:
                internalFormat = GL_RG32F;
                format = GL_RG;
                type = GL_FLOAT;
                break;
            case Format::R32F:
                internalFormat = GL_R32F;
                format = GL_RED;
                type = GL_FLOAT;
                break;
            case Format::Depth24Stencil8:
                internalFormat = GL_DEPTH24_STENCIL8;
                format = GL_DEPTH_STENCIL;
                type = GL_UNSIGNED_INT_24_8;
                break;
            case Format::RGBA16U:
            case Format::RGB16U:
            case Format::RG16U:
            case Format::RGBA16I:
            case Format::RGB16I:
            case Format::RG16I:
                invalid_enum(Format, mInfo.format);
        }

        ScopedBindGL thisBind(this);

        if (isTextureArray())
            glTexImage3D(getBindPoint(), 0, internalFormat, mInfo.width, mInfo.height, mInfo.arrayLayers, 0, format, type, nullptr);
        else
            glTexImage2D(getBindPoint(), 0, internalFormat, mInfo.width, mInfo.height, 0, format, type, nullptr);

        glTexParameteri(getBindPoint(), GL_TEXTURE_MIN_FILTER, mInfo.minFilter == Filter::Nearest ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(getBindPoint(), GL_TEXTURE_MAG_FILTER, mInfo.magFilter == Filter::Nearest ? GL_NEAREST : GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }

    TextureOpenGL::~TextureOpenGL() { glDeleteTextures(1, &mId); }

    void
    TextureOpenGL::updateImageData(int32_t x, int32_t y, int32_t layer, int32_t width, int32_t height, const uint8_t* rgba8UnormData, int32_t pitchInPixels)
    {
        ScopedBindGL thisBind(this);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, pitchInPixels);

        if (isTextureArray())
            glTexSubImage3D(getBindPoint(), 0, x, y, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba8UnormData);
        else
            glTexSubImage2D(getBindPoint(), 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba8UnormData);
    }

    void TextureOpenGL::readImageData(uint8_t* rgba8UnormDestination)
    {
        release_assert(mInfo.format == Format::RGBA8UNorm);

        ScopedBindGL thisBind(this);
        glGetTexImage(getBindPoint(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba8UnormDestination);
    }

    void TextureOpenGL::setFilter(Filter minFilter, Filter magFilter)
    {
        super::setFilter(minFilter, magFilter);

        ScopedBindGL thisBind(this);
        glTexParameteri(getBindPoint(), GL_TEXTURE_MIN_FILTER, mInfo.minFilter == Filter::Nearest ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(getBindPoint(), GL_TEXTURE_MAG_FILTER, mInfo.magFilter == Filter::Nearest ? GL_NEAREST : GL_LINEAR);
    }

    void TextureOpenGL::unbind(std::optional<GLuint> extra1, std::optional<GLuint>)
    {
        if (extra1)
            glActiveTexture(*extra1);

        glBindTexture(getBindPoint(), 0);
    }
    void TextureOpenGL::bind(std::optional<GLuint> extra1, std::optional<GLuint>)
    {
        if (extra1)
            glActiveTexture(*extra1);

        glBindTexture(getBindPoint(), mId);
    }

    GLenum TextureOpenGL::getBindPoint() const { return isTextureArray() ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D; }
}
