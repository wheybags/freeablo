#include "vertexbuffer.h"

namespace Render
{
    Buffer::Buffer(size_t sizeInBytes) : mSizeInBytes(sizeInBytes) { glGenBuffers(1, &mId); }

    Buffer::~Buffer() { glDeleteBuffers(1, &mId); }

    void Buffer::setData(void* data, size_t dataSizeInBytes)
    {
        // TODO: check mSizeInBytes to make sure this buffer can fit the data
        // In opengl this doesn't actually matter, the buffers are dynamically resized,
        // but it would be nice to enforce anyway.
        // Will do this once we have batch-splitting implemented.

        glBindBuffer(GL_COPY_WRITE_BUFFER, mId);
        glBufferData(GL_COPY_WRITE_BUFFER, dataSizeInBytes, data, GL_STATIC_DRAW);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }

    IndexBuffer::IndexBuffer(size_t count) : mBuffer(count * sizeof(uint16_t)) {}

    void IndexBuffer::setData(uint16_t* indices, size_t count) { mBuffer.setData(indices, count * sizeof(uint16_t)); }

    void IndexBuffer::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffer.getId()); }

    void IndexBuffer::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

    VertexBuffer::VertexBuffer(size_t count, const VertexLayout& layout) : mBuffer(count * layout.getSizeInBytes()), mLayout(layout) {}

    GLint VertexBuffer::setupAttributes(GLint locationIndex)
    {
#ifndef NDEBUG
        debug_assert(VertexArrayObject::currentlyBound);
#endif

        ScopedBind binder(this);

        size_t offset = 0;
        for (Format element : mLayout.getElements())
        {
            GLenum type = 0;
            GLint size = 0;
            GLboolean normalized = GL_FALSE;

            switch (element)
            {
                case Format::RGBA8U:
                    type = GL_UNSIGNED_BYTE;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGBA8UNorm:
                    type = GL_UNSIGNED_BYTE;
                    normalized = GL_TRUE;
                    size = 4;
                    break;
                case Format::RGBA32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGB32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 3;
                    break;
                case Format::RG32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 2;
                    break;
                case Format::R32F:
                    type = GL_FLOAT;
                    normalized = GL_FALSE;
                    size = 1;
                    break;
                case Format::RGBA16U:
                    type = GL_UNSIGNED_SHORT;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGB16U:
                    type = GL_UNSIGNED_SHORT;
                    normalized = GL_FALSE;
                    size = 3;
                    break;
                case Format::RG16U:
                    type = GL_UNSIGNED_SHORT;
                    normalized = GL_FALSE;
                    size = 2;
                    break;
                case Format::RGBA16I:
                    type = GL_SHORT;
                    normalized = GL_FALSE;
                    size = 4;
                    break;
                case Format::RGB16I:
                    type = GL_SHORT;
                    normalized = GL_FALSE;
                    size = 3;
                    break;
                case Format::RG16I:
                    type = GL_SHORT;
                    normalized = GL_FALSE;
                    size = 2;
                    break;
            }

            glVertexAttribPointer(locationIndex, size, type, normalized, mLayout.getSizeInBytes(), reinterpret_cast<void*>(offset));
            glVertexAttribDivisor(locationIndex, mLayout.getRate() == VertexInputRate::ByVertex ? 0 : 1);
            glEnableVertexAttribArray(locationIndex);

            offset += formatSize(element);
            locationIndex++;
        }

        return locationIndex;
    }

    void VertexBuffer::bind() { glBindBuffer(GL_ARRAY_BUFFER, mBuffer.getId()); }

    void VertexBuffer::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

    VertexArrayObject::VertexArrayObject(std::vector<size_t> bufferSizeCounts,
                                         std::vector<NonNullConstPtr<VertexLayout>> bindings,
                                         size_t indexBufferSizeInElements)
        : mBindings(bindings)
    {
        debug_assert(bufferSizeCounts.size() == bindings.size());

        glGenVertexArrays(1, &mVaoId);
        ScopedBind binder(this);

        GLint locationIndex = 0;
        for (size_t bindingIndex = 0; bindingIndex < mBindings.size(); bindingIndex++)
        {
            auto buffer = std::make_unique<VertexBuffer>(bufferSizeCounts[bindingIndex], *bindings[bindingIndex]);
            locationIndex = buffer->setupAttributes(locationIndex);
            mBuffers.emplace_back(buffer.release());
        }

        if (indexBufferSizeInElements > 0)
            mIndexBuffer = std::make_unique<IndexBuffer>(indexBufferSizeInElements);
    }

    VertexArrayObject::~VertexArrayObject() { glDeleteVertexArrays(1, &mVaoId); }

    void VertexArrayObject::bind()
    {
        glBindVertexArray(mVaoId);

#ifndef NDEBUG
        VertexArrayObject::currentlyBound = this;
#endif
    }

    void VertexArrayObject::unbind()
    {
        glBindVertexArray(0);

#ifndef NDEBUG
        VertexArrayObject::currentlyBound = nullptr;
#endif
    }

#ifndef NDEBUG
    VertexArrayObject* VertexArrayObject::currentlyBound = nullptr;
#endif
}
