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
        virtual ~BufferOpenGL();

        GLuint getId() { return mId; }

        virtual void setData(void* data, size_t dataSizeInBytes) override;

        virtual void bind(std::optional<GLenum> binding) override;
        virtual void unbind(std::optional<GLenum> binding) override;

    private:
        GLuint mId = 0;
    };
}
