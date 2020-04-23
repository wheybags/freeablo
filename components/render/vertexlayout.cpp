#include <misc/assert.h>
#include <render/vertexlayout.h>

namespace Render
{
    size_t formatSize(Format format)
    {
        switch (format)
        {
            case Format::RGBA8UNorm:
                return 4;
            case Format::RGBA32F:
                return 4 * 4;
            case Format::RGB32F:
                return 4 * 3;
            case Format::RG32F:
                return 4 * 2;
            case Format::R32F:
                return 4;
            case Format::RGBA16U:
                return 2 * 4;
            case Format::RGB16U:
                return 2 * 3;
            case Format::RG16U:
                return 2 * 2;
            case Format::RGBA16I:
                return 2 * 4;
            case Format::RGB16I:
                return 2 * 3;
            case Format::RG16I:
                return 2 * 2;
            case Format::Depth24Stencil8:
                break;
        }

        invalid_enum(Format, format);
    }

    VertexLayout::VertexLayout(std::initializer_list<Format> elements, VertexInputRate rate) : mElements(elements), mRate(rate)
    {
        mSizeInBytes = 0;
        for (const auto& elem : mElements)
            mSizeInBytes += formatSize(elem);
    }
}
