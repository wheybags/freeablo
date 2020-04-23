#pragma once
#include <cstddef>
#include <vector>

namespace Render
{
    enum class Format
    {
        RGBA8UNorm,
        RGBA32F,
        RGB32F,
        RG32F,
        R32F,
        RGBA16U,
        RGB16U,
        RG16U,
        RGBA16I,
        RGB16I,
        RG16I,
        Depth24Stencil8,
    };

    size_t formatSize(Format format);

    enum class VertexInputRate
    {
        ByVertex,
        ByInstance,
    };

    class VertexLayout
    {
    public:
        VertexLayout(VertexLayout&) = delete;
        VertexLayout(std::initializer_list<Format> elements, VertexInputRate rate);

        const std::vector<Format>& getElements() const { return mElements; }
        VertexInputRate getRate() const { return mRate; }
        size_t getSizeInBytes() const { return mSizeInBytes; }

    private:
        std::vector<Format> mElements;
        size_t mSizeInBytes = 0;
        VertexInputRate mRate = VertexInputRate::ByVertex;
    };
}
