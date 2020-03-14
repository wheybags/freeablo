#include <misc/assert.h>
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
        glBufferData(GL_COPY_WRITE_BUFFER, dataSizeInBytes, data, GL_STATIC_DRAW);
    }

    void BufferOpenGL::bind(std::optional<GLenum> binding)
    {
        debug_assert(binding.has_value());
        glBindBuffer(binding.value(), mId);
    }

    void BufferOpenGL::unbind(std::optional<GLenum> binding)
    {
        debug_assert(binding.has_value());
        glBindBuffer(binding.value(), 0);
    }
}