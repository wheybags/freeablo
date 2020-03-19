#pragma once
#include <glad/glad.h>
#include <render/OpenGL/scopedbindgl.h>
#include <render/buffer.h>

namespace Render
{
    class BufferOpenGL final : public BindableGL, public Buffer
    {
        using super = Buffer;

    public:
        explicit BufferOpenGL(size_t sizeInBytes);
        ~BufferOpenGL() override;

        GLuint getId() { return mId; }

        void setData(void* data, size_t dataSizeInBytes) override;

        void bind(std::optional<GLenum> binding) override;
        void unbind(std::optional<GLenum> binding) override;

    private:
        GLuint mId = 0;
        GLuint mBufferTextureId = 0;
    };
}
