#pragma once
#include <cstddef>

namespace Render
{
    class Buffer
    {
    public:
        Buffer(size_t sizeInBytes) : mSizeInBytes(sizeInBytes) {}

        virtual ~Buffer() = default;

        size_t getSizeInBytes() { return mSizeInBytes; }

        virtual void setData(void* data, size_t dataSizeInBytes) = 0;

    protected:
        size_t mSizeInBytes = 0;
    };
}