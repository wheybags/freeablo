#pragma once
#include <cstddef>
#include <cstdint>
#include <glad/glad.h>
#include <memory>
#include <misc/assert.h>
#include <misc/misc.h>
#include <vector>

namespace Render
{
    enum class Format
    {
        RGBA8U,
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
    };

    static size_t formatSize(Format format)
    {
        switch (format)
        {
            case Format::RGBA8U:
                return 4;
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
        }

        invalid_enum(Format, format);
    }

    enum class VertexInputRate
    {
        ByVertex,
        ByInstance,
    };

    class VertexLayout
    {
    public:
        VertexLayout(std::initializer_list<Format> elements, VertexInputRate rate) : mElements(elements), mRate(rate)
        {
            mSizeInBytes = 0;
            for (const auto& elem : mElements)
                mSizeInBytes += formatSize(elem);
        }

        const std::vector<Format>& getElements() const { return mElements; }
        VertexInputRate getRate() const { return mRate; }
        size_t getSizeInBytes() const { return mSizeInBytes; }

    private:
        std::vector<Format> mElements;
        size_t mSizeInBytes = 0;
        VertexInputRate mRate = VertexInputRate::ByVertex;
    };

    struct SpriteVertexMain
    {
        float vertex_position[3];
        float v_uv[2];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RGB32F,
                                           Format::RG32F,
                                       },
                                       VertexInputRate::ByVertex};

            debug_assert(layout.getSizeInBytes() == sizeof(SpriteVertexMain));

            return layout;
        }
    };

    struct SpriteVertexPerInstance
    {
        uint16_t v_imageSize[2];
        int16_t v_imageOffset[2];
        uint8_t v_hoverColor[4];
        uint16_t v_atlasOffset[3];

        static const VertexLayout& layout()
        {
            static VertexLayout layout{{
                                           Format::RG16U,
                                           Format::RG16I,
                                           Format::RGBA8UNorm,
                                           Format::RGB16U,
                                       },
                                       VertexInputRate::ByInstance};

            debug_assert(layout.getSizeInBytes() == sizeof(SpriteVertexPerInstance));

            return layout;
        }
    };

    class Bindable
    {
    public:
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };

    class ScopedBind
    {
    public:
        ScopedBind(Bindable& resource) : mResource(resource) { mResource.bind(); }

        ScopedBind(Bindable* resource) : ScopedBind(*resource) {}

        ~ScopedBind() { mResource.unbind(); }

    private:
        Bindable& mResource;
    };

    class VertexBuffer : public Bindable
    {
    public:
        VertexBuffer(size_t count, const VertexLayout& layout);

        // This method presumes that you have bound a VAO before calling it
        GLint setupAttributes(GLint locationIndex);

        GLuint getId() { return mId; }
        size_t getSizeInBytes() { return mSizeInBytes; }

        void setData(void* data, size_t dataSizeInBytes);

        virtual void bind() override;
        virtual void unbind() override;

    private:
        GLuint mId = 0;
        size_t mSizeInBytes = 0;
        const VertexLayout& mLayout;
    };

    class VertexArrayObject : public Bindable
    {
    public:
        VertexArrayObject(std::vector<size_t> bufferSizeCounts, std::vector<NonNullConstPtr<VertexLayout>> bindings);

        virtual void bind() override;
        virtual void unbind() override;

        std::vector<NonNullConstPtr<VertexLayout>> mBindings;
        std::vector<std::unique_ptr<VertexBuffer>> mBuffers;
        GLuint mVaoId;

#ifndef NDEBUG
        static VertexArrayObject* currentlyBound;
#endif
    };
}
