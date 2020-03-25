#include <render/OpenGL/renderinstanceopengl.h>
#include <render/OpenGL/textureopengl.h>

namespace Render
{
    TextureOpenGL::TextureOpenGL(RenderInstanceOpenGL& instance, const BaseTextureInfo& info) : super(instance, info)
    {
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
            case Format::RGBA16U:
            case Format::RGB16U:
            case Format::RG16U:
            case Format::RGBA16I:
            case Format::RGB16I:
            case Format::RG16I:
                invalid_enum(Format, mInfo.format);
        }

        ScopedBindGL thisBind(this);

        if (mInfo.arrayLayers == 1)
            glTexImage2D(getBindPoint(), 0, internalFormat, mInfo.width, mInfo.height, 0, format, type, nullptr);
        else
            glTexImage3D(getBindPoint(), 0, internalFormat, mInfo.width, mInfo.height, mInfo.arrayLayers, 0, format, type, nullptr);

        glTexParameteri(getBindPoint(), GL_TEXTURE_MIN_FILTER, mInfo.minFilter == Filter::Nearest ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(getBindPoint(), GL_TEXTURE_MAG_FILTER, mInfo.magFilter == Filter::Nearest ? GL_NEAREST : GL_LINEAR);
    }

    TextureOpenGL::~TextureOpenGL() { glDeleteTextures(1, &mId); }

    void TextureOpenGL::updateImageData(int32_t x, int32_t y, int32_t layer, int32_t width, int32_t height, const uint8_t* rgba8UnormData)
    {
        ScopedBindGL thisBind(this);

        if (mInfo.arrayLayers == 1)
            glTexSubImage2D(getBindPoint(), 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba8UnormData);
        else
            glTexSubImage3D(getBindPoint(), 0, x, y, layer, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba8UnormData);
    }

    void TextureOpenGL::unbind(std::optional<GLuint> extra1, std::optional<GLuint>)
    {
        if (extra1)
            glActiveTexture(*extra1);

        glBindTexture(getBindPoint(), mId);
    }
    void TextureOpenGL::bind(std::optional<GLuint> extra1, std::optional<GLuint>)
    {
        if (extra1)
            glActiveTexture(*extra1);

        glBindTexture(getBindPoint(), mId);
    }

    GLenum TextureOpenGL::getBindPoint() const { return mInfo.arrayLayers == 1 ? GL_TEXTURE_2D : GL_TEXTURE_2D_ARRAY; }
}
