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

        void bind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;
        void unbind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;

    private:
        GLuint mId = 0;
    };

    class BufferSliceOpenGL final : public BindableGL
    {
    public:
        explicit BufferSliceOpenGL(BufferSlice slice) : slice(slice) {}

        BufferSlice slice;

        void bind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;
        void unbind(std::optional<GLuint> extra1, std::optional<GLuint> extra2) override;
    };
}
