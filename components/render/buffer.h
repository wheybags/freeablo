#pragma once
#include <cstddef>

namespace Render
{
    class Buffer
    {
    public:
        Buffer(Buffer&) = delete;
        explicit Buffer(size_t sizeInBytes) : mSizeInBytes(sizeInBytes) {}

        virtual ~Buffer() = default;

        size_t getSizeInBytes() { return mSizeInBytes; }

        virtual void setData(void* data, size_t dataSizeInBytes) = 0;

    protected:
        size_t mSizeInBytes = 0;
    };

    struct BufferSlice
    {
        Buffer* buffer;
        size_t offset;
        size_t length;

        BufferSlice(Buffer* buffer) : buffer(buffer), offset(0), length(buffer->getSizeInBytes()) {}
        BufferSlice(Buffer* buffer, size_t offset, size_t length) : buffer(buffer), offset(offset), length(length) {}
    };
}