#include <misc/assert.h>
#include <misc/misc.h>
#include <render/OpenGL/bufferopengl.h>

namespace Render
{
    BufferOpenGL::BufferOpenGL(size_t sizeInBytes) : super(sizeInBytes) { glGenBuffers(1, &mId); }

    BufferOpenGL::~BufferOpenGL() { glDeleteBuffers(1, &mId); }

    void BufferOpenGL::setData(void* data, size_t dataSizeInBytes)
    {
        // TODO: check mSizeInBytes to make sure this buffer can fit the data
        // In opengl this doesn't actually matter, the buffers are dynamically resized,
        // but it would be nice to enforce anyway.
        // Will do this once we have batch-splitting implemented.

        ScopedBindGL thisBind(this, GL_COPY_WRITE_BUFFER);
        glBufferData(GL_COPY_WRITE_BUFFER, dataSizeInBytes, data, GL_DYNAMIC_DRAW);
    }

    void BufferOpenGL::bind(std::optional<GLuint> extra1, std::optional<GLuint>)
    {
        debug_assert(extra1.has_value());
        glBindBuffer(extra1.value(), mId);
    }

    void BufferOpenGL::unbind(std::optional<GLuint> extra1, std::optional<GLuint>)
    {
        debug_assert(extra1.has_value());
        glBindBuffer(extra1.value(), 0);
    }

    void BufferSliceOpenGL::bind(std::optional<GLuint> extra1, std::optional<GLuint> extra2)
    {
        debug_assert(extra1.has_value() && extra2.has_value());
        auto buffer = safe_downcast<BufferOpenGL*>(slice.buffer);
        glBindBufferRange(*extra1, *extra2, buffer->getId(), slice.offset, slice.length);
    }

    void BufferSliceOpenGL::unbind(std::optional<GLuint> extra1, std::optional<GLuint> extra2)
    {
        debug_assert(extra1.has_value() && extra2.has_value());
        glBindBufferRange(*extra1, *extra2, 0, 0, 0);
    }
}